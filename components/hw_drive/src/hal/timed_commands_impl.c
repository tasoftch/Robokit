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

#include "hal/timed_commands_impl.h"

#include <portmacro.h>

#define __TEST__
#include "private/thread_safety.h"

#include "timed_commands.h"

static S_T_cmd _full_commands_stack[ ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT ] = {0};
static uint16_t _full_commands_stack_free_count = ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT;

static S_T_chain _commands_chain[ROBOKIT_TIMED_COMMAND_LISTS_COUNT] = {0};
static uint8_t _commands_chain_free_count = ROBOKIT_TIMED_COMMAND_LISTS_COUNT;


S_T_cmd *tc_command_alloc(void) {
	static uint16_t index = 0;
	S_T_cmd *cmd = NULL;

	if(_full_commands_stack_free_count > 0) {
		ROBOKIT_THREAD_SAFE_BLOCK_START()
		while(_full_commands_stack[index].reserved1 & TCMD_FLAG_RESERVED) {
			index++;
			if(index >= ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT) {
				index = 0;
			}
		}

		_full_commands_stack_free_count--;
		cmd = &_full_commands_stack[ index ];
		cmd->reserved1 |= TCMD_FLAG_RESERVED;
		ROBOKIT_THREAD_SAFE_BLOCK_END();
	}
	return cmd;
}

void tc_command_free(S_T_cmd *tc) {
	if(tc != NULL && tc->reserved1 & TCMD_FLAG_RESERVED) {
		ROBOKIT_THREAD_SAFE( tc->reserved1 &= ~TCMD_FLAG_RESERVED; );
		ROBOKIT_THREAD_SAFE( _full_commands_stack_free_count++ );
	}
}

uint16_t tc_command_get_available(void) {
	return _full_commands_stack_free_count;
}


void tc_command_init(S_T_cmd *tc) {
	if(tc != NULL && tc->flags & TCMD_FLAG_RESERVED) {
		tc->reserved1 |= TCMD_FLAG_PREPARE;
	}
}


S_T_chain *tc_chain_alloc(void) {
	static uint8_t index = 0;
	S_T_chain *cmd = NULL;

	if(_commands_chain_free_count > 0) {
		ROBOKIT_THREAD_SAFE_BLOCK_START()
		while(_commands_chain[index].flags & TCMD_FLAG_RESERVED) {
			index++;
			if(index >= ROBOKIT_TIMED_COMMAND_LISTS_COUNT) {
				index = 0;
			}
		}

		_commands_chain_free_count--;
		cmd = &_commands_chain[ index ];
		cmd->flags |= TCMD_FLAG_RESERVED;
		ROBOKIT_THREAD_SAFE_BLOCK_END();
	}
	return cmd;
}

void tc_chain_free(S_T_chain *tc) {
	if(tc != NULL && tc->flags & TCMD_FLAG_RESERVED) {
		ROBOKIT_THREAD_SAFE( tc->flags &= ~TCMD_FLAG_RESERVED );
		ROBOKIT_THREAD_SAFE( _commands_chain_free_count++ );
	}
}

uint8_t tc_chain_get_available(void) {
	return _commands_chain_free_count;
}

void tc_chain_init(S_T_chain *tc) {
	if(tc != NULL && tc->flags & TCMD_FLAG_RESERVED) {
		tc->flags |= TCMD_FLAG_PREPARE;
	}
}