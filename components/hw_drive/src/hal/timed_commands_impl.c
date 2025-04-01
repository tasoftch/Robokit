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

#include <stddef.h>

#include "timed_commands.h"

static S_T_cmd _full_commands_stack[ ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT ] = {0};
static uint16_t _full_commands_stack_free_count = ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT;

S_T_cmd *tc_alloc_command(void) {
	static uint16_t index = 0;
	if(_full_commands_stack_free_count > 0) {
		while(_full_commands_stack[index].flags & TCMD_FLAG_RESERVED) {
			index++; // TODO: make thread safe
			if(index >= ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT) {
				index = 0;
			}
		}

		_full_commands_stack_free_count--; // TODO: make thread safe
		S_T_cmd *cmd = &_full_commands_stack[ index ];
		cmd->flags |= TCMD_FLAG_RESERVED; // TODO: make thread safe
		return cmd;
	}
	return NULL;
}

void tc_free_command(S_T_cmd *tc) {
	if(tc != NULL && tc->flags & TCMD_FLAG_RESERVED) {
		_full_commands_stack_free_count++;
		tc->flags &= ~TCMD_FLAG_RESERVED; // TODO: make thread safe
	}
}

uint16_t tc_get_available_command_stack_count(void) {
	return _full_commands_stack_free_count;
}
