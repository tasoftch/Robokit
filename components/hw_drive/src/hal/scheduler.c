/*
 * MIT License
 *
 * Copyright (c) 2024 Th. Abplanalp, F. Romer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "hal/scheduler.h"

#include "device.h"
#include <esp_log.h>

#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <freertos/task.h>
#include <private/robokit_log.h>

static volatile S_command command_stack[ROBOKIT_COMMAND_STACK_SIZE];
static volatile uint8_t command_stack_head = 0;

static F_command_callback _callback_fn_list[ROBOKIT_MAX_SCHEDULED_COMMANDS] = {0};
static QueueHandle_t commandQueue;

static void(*_callbacks[ ROBOKIT_MAX_SCHEDULED_COMMANDS ])(void);
static uint8_t _callback_count = 0;

void __robokit_register_loop_callback(void(*cb)(void)) {
	if(_callback_count < ROBOKIT_MAX_SCHEDULED_COMMANDS) {
		_callbacks[ _callback_count++ ] = cb;
	} else {
		ROBOKIT_LOGE("Maximum number of init modules exceeded");
	}
}

/**
 * @brief Hard fault Handler
 *
 * Falls irgendetwas schiefgeht, wird der Task in diese Funktion geleitet.
 * Mit der Endlosschleife verbleibt er da für Debug Zwecke.
 */
void hal_error_handler() {
	ROBOKIT_LOGE("!! Hard fault !!");
	while (1) {
	}
}

/**
 * @brief Ausführen des Kommandos in der Hardware.
 * @param cmd S_Command* Kommando, welches ausgeführt werden soll.
 */
void hal_task_handler(S_command *cmd) {
	uint8_t flags=0;
	_callback_fn_list[cmd->cmd](cmd, E_SCHEDULE_MODE_PERFORM, &flags);
}

/**
 * @brief Haupt Task für das Einschleusen der Kommandos
 *
 * Dieser Task nimmt die Kommandos aus der Warteschlange und führt sie auf der Hardware aus.
 * Er sucht dabei nach einer registrierten Callbackfunktion für ein Kommando und ruft sie auf.
 *
 * @param parameters void *
 */
void _robokit_task_handler(void *parameters) {
	while (1) {
		uint8_t cmd_idx=0;
		ROBOKIT_LOGI("Waiting for command...");
		xQueueReceive(commandQueue, &cmd_idx, portMAX_DELAY);
		ROBOKIT_LOGI("Command %d received at index %d", command_stack[cmd_idx].cmd, cmd_idx);

		if(command_stack[cmd_idx].cmd != E_COMMAND_NONE) {
			S_command *cmd = (S_command *) & command_stack[cmd_idx];
			hal_task_handler(cmd);
		}
	}
}

/**
 * @brief A 100Hz Task that updates all modules that registered a sensor loop function
 *
 * The task updates all sensor loops of the modules.
 *
 * @param parameters void * Not used
 */
void _robokit_task_handler_peripherals(void *parameters) {
	const TickType_t period = pdMS_TO_TICKS(10);  // 10 ms = 100 Hz
	TickType_t last_wake_time = xTaskGetTickCount();

	while (1) {
		for(int e=0;e<_callback_count;e++) {
			_callbacks[e]();
		}

		vTaskDelayUntil(&last_wake_time, period);
	}
}

/**
 * @brief Wird von device_init() aufgerufen und setzt die Umgebung auf.
 *
 * Die Hardware läuft in einem separaten Task mit hoher Priorität.
 * Gleichzeitig wird ein weiterer Task erzeugt, welcher sich um zyklische
 * Aufgaben in der Hardware kümmert.
 */
void _scheduler_init(void) {
	commandQueue = xQueueCreate(8, 1);
	if(!commandQueue) {
		ROBOKIT_LOGE("Could not create command queue");
		hal_error_handler();
	}

	xTaskCreate(
		_robokit_task_handler,
		"rob_tahdl",
		configMINIMAL_STACK_SIZE+2000,
		((void*) 1),
		tskIDLE_PRIORITY+5,
		NULL
		);

	xTaskCreate(
		_robokit_task_handler_peripherals,
		"rob_peri",
		configMINIMAL_STACK_SIZE+2000,
		((void*) 1),
		tskIDLE_PRIORITY+3,
		NULL
		);
}

/**
 * @brief Registriert Callback Funktionen für die Interaktion
 *
 * Die Callback Funktionen dürfen nicht blockieren und müssen möglichst schnell beendet werden.
 * Jede Callback Funktion wird zweimal aufgerufen.
 * Das erste Mal mit dem Modus E_SCHEDULE_MODE_PRECHECK. Sie muss prüfen, ob das Kommando gültig ist und
 * eingeschleust werden darf. Ebenfalls darf das Kommando noch angepasst werden falls nötig.
 * Das passiert noch im Task, welcher push_command aufruft.
 * Das zweite Mal mit dem Modus E_SCHEDULE_MODE_PERFORM. Jetzt muss die Callback das Kommando in der Hardware umsetzen.
 * Dies geschieht im Hardware Task, welcher die höchste Priorität hat. Deshalb muss die Funktion so schnell wie möglich
 * zurückkehren.
 *
 * @param cmd T_cmd Kommando Nummer, unter welcher die Handler Funktion abgelegt wird.
 * @param cb void*
 * @return 1 im Erfolgsfall, sonst 0
 */
uint8_t robokit_register_command_fn(T_cmd cmd, F_command_callback cb) {
	if(cmd < ROBOKIT_MAX_SCHEDULED_COMMANDS) {
		if(_callback_fn_list[cmd] != NULL)
			return 0;
		_callback_fn_list[cmd] = cb;
		return 1;
	}
	return 0;
}

/**
 * @brief Schleust ein neues Kommando in die Hardware ein.
 *
 * Diese Funktion bildet die Hauptschnittstelle zwischen GUI und Hardware.
 * Sie trennt beide Tasks voneinander.
 *
 * @param [in] cmd S_Command* Neues Kommando
 * @param [in] flags Integer Flags
 * @return 1|0
 * @see E_COMMAND_FLAG_* Konstanten
 */
robokit_err_t robokit_push_command(S_command *cmd, uint8_t flags) {
	T_cmd tcmd = cmd->cmd;
	uint8_t cmd_idx;

	if(_callback_fn_list[tcmd] == NULL) {
		ROBOKIT_LOGE("No command found %d.", cmd->cmd);
		return ROBOKIT_ERR_UNKNOWN_COMMAND;
	}

	_callback_fn_list[tcmd](cmd, E_SCHEDULE_MODE_PRECHECK, &flags);
	if(flags == 0xFF)
		return ROBOKIT_ERR_PRECHECK_FAILED;

	if( robokit_get_free_stack_count() < 1 && !(flags & E_COMMAND_FLAG_BLOCK) ) {
		ROBOKIT_LOGW("Stack is full");
		return ROBOKIT_ERR_FULL_PUSH_STACK;
	}

	taskDISABLE_INTERRUPTS();
	cmd_idx = command_stack_head++;
	command_stack[cmd_idx] = *cmd;
	if(command_stack_head >= ROBOKIT_COMMAND_STACK_SIZE) {
		command_stack_head = 0;
	}
	xQueueSend(commandQueue, &cmd_idx, portMAX_DELAY);
	taskENABLE_INTERRUPTS();

	ROBOKIT_LOGI("Command %d enqueued at index %d.", cmd->cmd, cmd_idx);
	return ROBOKIT_OK;
}

/**
 * @brief Liefert die Anzahl noch freier Kommandos, welche eingespeist werden können.
 * @return uint8_t
 * @see robokit_push_command();
 */
uint8_t robokit_get_free_stack_count(void) {
	return uxQueueSpacesAvailable(commandQueue);
}