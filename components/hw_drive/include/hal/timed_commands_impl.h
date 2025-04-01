/*
 * MIT License
 *
 * Copyright (c) 2025 Th. Abplanalp, F. Romer
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

#ifndef TIMED_COMMANDS_IMPL_H
#define TIMED_COMMANDS_IMPL_H

#include <stdint.h>
#include <main_commands.h>
#include <modules/specs.h>

/*
 * A command list is a construct that holds one or more native commands
 * in a time scheduled sequence.
 *
 * You can add commands to the list by defining an interval (in ms) before or after the command
 * When the list is prepared, you can push it to the firmware to be executed immediately.
 *
 * There exist several commands to control the lists and its executation in the firmware.
 */

// Specifies the amount of independent lists, that can be prepared and executed.
#define ROBOKIT_TIMED_COMMAND_LISTS_COUNT 5

// A list has only as many items as required.
// A list with one command only uses one stack item. another with 130 uses 130.
// Therefore, the stack items can be organized accordingly.
#define ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT 5

typedef struct _S_T_cmd {
	// positive intervals in ms mean waiting after command,
	// negative intervals in ms mean waiting before command
	// You can wait for maximum 30 seconds before or after the command.
	int16_t interval_ms;

	// The flags for pushing the command
	uint8_t flags;

	// unused field.
	uint8_t reserved1;

	// The command to be pushed to the firmware
	S_command command;

	// Points to the next command
	struct _S_T_cmd *next_command;
} S_T_cmd;


typedef struct {
	// The length of the chain (count of commands in chain)
	uint8_t length;

	// Estimated duration of the list until completed
	// This value is decrementing while the firmware is handling the chain
	uint16_t duration_ms;

	// For internal usage
	uint8_t flags;

	// The first command to be executed by the list
	S_T_cmd *first_command;

	// The last command in the list
	S_T_cmd *last_command;
} S_T_chain;

typedef enum {
	TCMD_FLAG_NONE							= 0,
	TCMD_FLAG_FREE							= 1<<0,
	TCMD_FLAG_RESERVED						= 1<<1,
	TCMD_FLAG_RUNNING						= 1<<2,
} E_internal_T_chain_flags;

// Returns the number of available (free, not in use) chains.
uint8_t tc_get_available_chain_count(void);

S_T_cmd *tc_alloc_command(void);
void tc_free_command(S_T_cmd *tc);
uint16_t tc_get_available_command_stack_count(void);


#endif //TIMED_COMMANDS_IMPL_H
