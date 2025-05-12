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

#include "device.h"

#include <stdio.h>
#include <string.h>
#include <private/robokit_log.h>
#include <modules/robokit_module.h>
#include "esp_partition.h"

extern void _scheduler_init(void);
extern void myval(void);

static void(*_callbacks[ ROBOKIT_MAX_SCHEDULED_COMMANDS ])(void);
static uint8_t _callback_count = 0;

static char serial_number[7] = {0};

// Init function from modules macro ROBOKIT_MODULE_INIT
void __robokit_register_init_callback(void(*cb)(void)) {
	if(_callback_count < ROBOKIT_MAX_SCHEDULED_COMMANDS) {
		_callbacks[ _callback_count++ ] = cb;
	} else {
		ROBOKIT_LOGE("Maximum number of init modules exceeded");
	}
}

/**
  * @brief Init Verteilung in der Hardware
  */
void device_init(void) {
	for(int e=0;e<ROBOKIT_MAX_SCHEDULED_COMMANDS;e++) {
		if(_callbacks[e])
			_callbacks[e]();
	}

	_scheduler_init();
}



static void read_serial_number() {
	const esp_partition_t* part = esp_partition_find_first(
		ESP_PARTITION_TYPE_DATA, 0x40, "serial");

	if (part != NULL) {
		esp_partition_read(part, 0, serial_number, sizeof(serial_number) - 1);
		if(strlen(serial_number) != 6) {
			strcpy(serial_number, "XXXXXX");
		}
	} else {
		strcpy(serial_number, "??????");
	}

	ROBOKIT_LOGI("Serial number: %s", serial_number);
}


const char *device_serial(void) {
	return serial_number;
}



ROBOKIT_MODULE_INIT() {
	read_serial_number();
}