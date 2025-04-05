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

#include "timed_commands.h"

#include <robokit_err.h>

robokit_err_t robokit_tc_get_empty_chain(robokit_chain_ref_t *chain) ROBOKIT_WL_PACKAGE( 3.4 ) {
	if(tc_chain_get_available() > 0) {
		if(chain) {
			*chain = tc_chain_alloc();
			tc_chain_init(*chain);
		}
		return ROBOKIT_OK;
	}
	return ROBOKIT_ERR_TC_NO_CHAIN_MEM;
}

uint8_t robokit_tc_push_command(
 S_T_chain *chain,
 int16_t timeout_ms,
 S_command command,
 uint8_t flags
) ROBOKIT_WL_PACKAGE( 3.4 ) {

	return ROBOKIT_OK;
}

robokit_err_t robokit_tc_push_callback(S_T_chain *chain, void (*callback)(void)) {

	return ROBOKIT_OK;
}

robokit_err_t robokit_tc_discard_list(S_T_chain *chain) {

	return ROBOKIT_OK;
}