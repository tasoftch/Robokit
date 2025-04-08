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

#ifndef TIMED_COMMANDS_H
#define TIMED_COMMANDS_H

#include <main_commands.h>
#include <hal/timed_commands_impl.h>
#include <robokit_err.h>

typedef S_T_chain * robokit_chain_ref_t;

/**
 * @brief Returns a free command chain to be filled by software.
 *
 * Please note that this function return 0 and chain = NULL if no chain is available.
 *
 * @param chain The memory space to alloc an internal pointer the chain in it.
 *
 * @return If no chain available, this function returns 0 otherwise 1
 */
robokit_err_t robokit_tc_get_empty_chain(robokit_chain_ref_t *chain) ROBOKIT_WL_PACKAGE( 3.4 );

// Use this function to add timed commands to a valid chain.
// If the list is not ready to receive commands, this function returns
// The return values are the same as described in robokit_push_command().
// The only exception is E_PUSH_CHAIN_FULL if the chain can not accept further commands (Stack, occupied)
robokit_err_t robokit_tc_push_command(
 S_T_chain *chain,
 int16_t timeout_ms,
 S_command *command,
 uint8_t flags
) ROBOKIT_WL_PACKAGE( 3.4 );

// Pushes a callback to the list.
// Use the callbacks to get informed when the list passes a specific position.
robokit_err_t robokit_tc_push_callback(S_T_chain *chain, void (*callback)(void));

/*
 * @brief Pushes the command chain to the firmware, which will start to execute it
 */
robokit_err_t robokit_tc_push_command_list(S_T_chain *chain, uint8_t flags) ROBOKIT_WL_PACKAGE( 3.4 );

robokit_err_t robokit_tc_discard_list(S_T_chain *chain);

// Make specific commands to control the device

robokit_err_t robokit_make_command_callback(S_command *command, void (*callback)(void));
#endif //TIMED_COMMANDS_H
