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

		S_T_cmd *cmd = tc->first_command;
		while(cmd) {
			tc_command_free(cmd);
			cmd = cmd->next_command;
		}
		tc->first_command = NULL;
		tc->last_command = NULL;
		tc->length = 0;
	}
}

uint8_t tc_chain_get_available(void) {
	return _commands_chain_free_count;
}

void tc_chain_init(S_T_chain *tc) {
	if(tc != NULL && tc->flags & TCMD_FLAG_RESERVED) {
		tc->flags |= TCMD_FLAG_PREPARE;
		tc->first_command = NULL;
		tc->last_command = NULL;
		tc->length = 0;
	}
}

robokit_err_t tc_chain_push_command(
 S_T_chain *chain,
 int16_t timeout_ms,
 S_command *command,
 uint8_t flags
) ROBOKIT_WL_PACKAGE( 3.4 ) {
	if(chain && command && chain->flags & TCMD_FLAG_PREPARE) {
		chain->duration_ms += timeout_ms < 0 ? -timeout_ms : timeout_ms;
		S_T_cmd *cmd = tc_command_alloc();
		if(!cmd)
			return ROBOKIT_ERR_TC_NO_CHAIN_MEM;

		cmd->interval_ms = timeout_ms;
		cmd->flags = flags;
		cmd->command = *command;

		if(!chain->first_command) {
			chain->first_command = cmd;
			chain->last_command = cmd;
			chain->length = 1;
		} else {
			chain->length++;
			chain->last_command->next_command = cmd;
			chain->last_command = cmd;
		}
		return ROBOKIT_OK;
	}
	return ROBOKIT_ERR_TC_NO_CHAIN_MEM;
}

robokit_err_t tc_chain_pop_command(S_T_chain *chain) {
	if(chain && chain->flags & TCMD_FLAG_PREPARE) {
		if(chain->first_command == chain->last_command) {
			chain->last_command = chain->first_command = NULL;
			chain->length = 0;
		} else {
			chain->length--;
			S_T_cmd *cmd = chain->first_command;
			while(cmd->next_command != chain->last_command) {
				cmd = cmd->next_command;
			}
			tc_command_free(chain->last_command);
			chain->last_command = cmd;
		}
		return ROBOKIT_OK;
	}
	return ROBOKIT_FAIL;
}