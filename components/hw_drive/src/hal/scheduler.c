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
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#define ROBOKIT_MAX_SCHEDULED_COMMANDS 32

static F_command_callback _callback_fn_list[ROBOKIT_MAX_SCHEDULED_COMMANDS] = {0};
static QueueHandle_t commandQueue;


void hal_error_handler() {
	// If there is any problem in the HW, call this function.
	// It contains an endless loop to holt cpu for debug.
	while (1) {
		;
	}
}


void _scheduler_init(void) {
	commandQueue = xQueueCreate(8, sizeof(S_command));
	if(!commandQueue) {
		printf("** ERROR: Could not initialize Queue.");
		hal_error_handler();
	}
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

	if(_callback_fn_list[tcmd] == NULL)
		return E_PUSH_STATUS_UNKNOWN_COMMAND;

	_callback_fn_list[tcmd](cmd, E_SCHEDULE_MODE_PRECHECK, &flags);

	if( robokit_get_free_stack_count() < 1 ) {

		return E_PUSH_STATUS_STACK_FULL;
	}


	return 0;
}

/* Returns how many commands can be added set. */
uint8_t robokit_get_free_stack_count(void) {
	return uxQueueSpacesAvailable(commandQueue);
}