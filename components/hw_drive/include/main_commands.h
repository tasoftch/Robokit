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

#ifndef commands_h
#define commands_h

#include <config.h>

typedef uint8_t T_cmd;

enum {
	E_COMMAND_NONE = 0,							// None or invalid command

	/* DRIVE COMMANDS */
	E_COMMAND_VECTOR,							// Drive using arg1 as X vector
												// and arg2 as Y vector.

	/* LED COMMANDS */
	E_COMMAND_LED_SETUP,						// Choose a LED number and set its color
	E_COMMAND_LED_FLUSH,						// Sends all LED setups to strip

	E_COMMAND_FAL,								// Call follow a line sensor command

	E_COMMAND_TEST = ROBOKIT_MAX_SCHEDULED_COMMANDS - 1
};

enum {
	E_COMMAND_FLAG_BLOCK                = 1<<0,		// Wait until command is enqueued
	E_COMMAND_FLAG_WAIT_UNTIL_DONE      = 1<<1,		// Wait until command is done by hardware
	E_COMMAND_FLAG_IMU                  = 1<<2		// Enables IMU
};

enum {
	E_PUSH_STATUS_OK,
	E_PUSH_STATUS_UNKNOWN_COMMAND,
	E_PUSH_STATUS_STACK_FULL,
	E_PUSH_PRECHECK_FAILED,

	E_PUSH_CHAIN_FULL
};

typedef struct {
	uint8_t cmd;                // Eindeutige Kennung des Kommandos
	uint8_t data[7];			// Datencontainer
} S_command;

#endif /* commands_h */
