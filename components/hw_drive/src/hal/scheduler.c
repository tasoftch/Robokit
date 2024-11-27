// Copyright (c) 2024 Th. Abplanalp
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the “Software”), to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
// CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.

//
// Created by Thomas Abplanalp on 15.11.24.

#include "scheduler.h"

#include <device.h>
#include <esp_log.h>
#include <fal.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "robokit_log.h"

static volatile S_command command_stack[ROBOKIT_COMMAND_STACK_SIZE];
static volatile uint8_t command_stack_head = 0;

static F_command_callback _callback_fn_list[ROBOKIT_MAX_SCHEDULED_COMMANDS] = {0};
static QueueHandle_t commandQueue;

static volatile uint8_t peripheralsStatus = 0;

void hal_error_handler() {
	// If there is any problem in the HW, call this function.
	// It contains an endless loop to holt cpu for debug.
	ROBOKIT_LOGE("!! Hard fault !!");
	while (1) {
		;
	}
}

void hal_task_handler(S_command *cmd) {
	uint8_t flags=0;
	_callback_fn_list[cmd->cmd](cmd, E_SCHEDULE_MODE_PERFORM, &flags);
}

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
 * A 50Hz Task that updates the drive by either follow a line or imu sensors
 *
 * @param parameters void * Not used
 */
void _robokit_task_handler_peripherals(void *parameters) {
	const TickType_t period = pdMS_TO_TICKS(20);  // 20 ms = 50 Hz
	TickType_t last_wake_time = xTaskGetTickCount();

	while (1) {
		fal_update();

		vTaskDelayUntil(&last_wake_time, period);
	}
}


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

uint8_t robokit_register_command_fn(T_cmd cmd, F_command_callback cb) {
	if(cmd < ROBOKIT_MAX_SCHEDULED_COMMANDS) {
		if(_callback_fn_list[cmd] != NULL)
			return 0;
		_callback_fn_list[cmd] = cb;
		return 1;
	}
	return 0;
}

uint8_t robokit_push_command(S_command *cmd, uint8_t flags) {
	T_cmd tcmd = cmd->cmd;
	uint8_t cmd_idx;

	if(_callback_fn_list[tcmd] == NULL) {
		ROBOKIT_LOGE("No command found %d.", cmd->cmd);
		return E_PUSH_STATUS_UNKNOWN_COMMAND;
	}


	_callback_fn_list[tcmd](cmd, E_SCHEDULE_MODE_PRECHECK, &flags);
	if(flags == 0xFF)
		return E_PUSH_PRECHECK_FAILED;

	if( robokit_get_free_stack_count() < 1 && !(flags & E_COMMAND_FLAG_BLOCK) ) {
		ROBOKIT_LOGW("Stack is full");
		return E_PUSH_STATUS_STACK_FULL;
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
	return 1;
}

/* Returns how many commands can be added set. */
uint8_t robokit_get_free_stack_count(void) {
	return uxQueueSpacesAvailable(commandQueue);
}