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
#include <robokit_err.h>
#include <modules/specs.h>
#include <stddef.h>

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
#ifdef __TEST__
#define ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT 5
#else
#define ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT 200
#endif

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
	TCMD_FLAG_PREPARE						= 1<<0,
	TCMD_FLAG_RESERVED						= 1<<1,
	TCMD_FLAG_RUNNING						= 1<<2,
} E_internal_T_chain_flags;

// Returns the number of available (free, not in use) chains.
uint8_t tc_get_available_chain_count(void);

S_T_cmd *tc_command_alloc(void);
void tc_command_free(S_T_cmd *tc);
uint16_t tc_command_get_available(void);

void tc_command_init(S_T_cmd *tc);

// Gets the next free chain.
// Returns NULL, if no chain is available.
S_T_chain *tc_chain_alloc(void);
void tc_chain_free(S_T_chain *tc);
uint8_t tc_chain_get_available(void);

void tc_chain_init(S_T_chain *tc);

robokit_err_t tc_chain_push_command(
	S_T_chain *chain,
	int16_t timeout_ms,
	S_command *command,
	uint8_t flags
) ROBOKIT_WL_PACKAGE( 3.4 );

robokit_err_t tc_chain_pop_command(S_T_chain *chain);


// Runtime

// This function gets called by the internal timer (triggered by tc_runtime_update()).
// Anythime that a command needs to be performed, this function gets called with the actual command.
void tc_runtime_command_handler(void (*handler)(S_command *command, uint8_t flags));

void tc_runtime_make_current_chain(S_T_chain *chain);
S_T_chain *tc_runtime_current_chain(void);

void tc_runtime_abort_current_chain(void);

robokit_err_t tc_runtime_replace_chain(S_T_chain *chain);

uint8_t tc_runtime_free_buffer_count(void);
robokit_err_t tc_runtime_append_command_list(S_T_chain *chain);

// Moves the time forward by ms
void tc_runtime_update(uint8_t ms);

#endif //TIMED_COMMANDS_IMPL_H
