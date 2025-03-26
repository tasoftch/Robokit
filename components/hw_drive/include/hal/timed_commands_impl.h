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

#define ROBOKIT_TIMED_COMMAND_SET_COUNT 5

#define ROBOKIT_TIMED_COMMAND_CHAIN_LENGTH 50

typedef struct {
	// positive intervals in ms mean waiting after command,
	// negative intervals in ms mean waiting before command
	int16_t interval_ms;

	// The flags for pushing the command
	uint8_t flags;

	// The command to be pushed to the firmware
	S_command command;
} S_T_cmd;


typedef struct {
	// The length of the chain (count of commands in chain)
	uint8_t length;

	// Estimated duration of the list until completed
	// This value is decrementing while the firmware is handling the chain
	uint16_t duration_ms;

	// For internal usage
	uint8_t flags;

	// The list of the commands assigned to the chain
	S_T_cmd *tcmd_list;
} S_T_chain;

typedef enum {
	TCMD_FLAG_NONE							= 0,
	TCMD_FLAG_FREE							= 1<<0,
	TCMD_FLAG_RESERVED						= 1<<1,
	TCMD_FLAG_RUNNING						= 1<<2,
} E_internal_T_chain_flags;

// Returns a free command chain to be filled by software.
// If no chain available, this function returns 0 otherwise 1
uint8_t robokit_tcommand_get_empty_chain(S_T_chain *chain) ROBOKIT_WL_PACKAGE( 3.4 );

// Returns the number of available (free, not in use) chains.
uint8_t robokit_tcommand_get_available_chain_count(void);

// Use this function to add timed commands to a valid chain.
// If the list is not ready to receive commands, this function returns
// The return values are the same as described in robokit_push_command().
// The only exception is E_PUSH_CHAIN_FULL if the chain can not accept further commands (Maximum defined by ROBOKIT_TIMED_COMMAND_CHAIN_LENGTH)
uint8_t robokit_tcommand_push_command(S_T_chain *chain, int16_t timeout_ms, S_command command, uint8_t flags) ROBOKIT_WL_PACKAGE( 3.4 );

// This function creates a command to be pushed to the firmware's main function robokit_push_command()
// As soon as the command gets into the firmware, it will be processed in time.
// The firmware will automatically push the commands in the chain accordingly to their timeout.
uint8_t robokit_make_chain_timer_command(S_command *cmd, S_T_chain *my_chain);
#endif //TIMED_COMMANDS_IMPL_H
