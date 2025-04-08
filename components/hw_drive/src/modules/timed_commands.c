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
#include <stdlib.h>
#include <modules/robokit_module.h>

enum {
	E_COMMAND_MODE_CALLBACK = 1,
	E_COMMAND_MODE_CHAIN = 2
};

typedef struct {
	T_cmd cmd;
	// Mode E_COMMAND_MODE_CALLBACK = simple callback, Mode E_COMMAND_MODE_CHAIN = timed chain
	uint8_t mode;
	uint8_t reserved1;
	uint8_t reserved2;
	void (*callback)(void);
} _S_command_callback;

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

robokit_err_t robokit_tc_push_command(
 S_T_chain *chain,
 int16_t timeout_ms,
 S_command *command,
 uint8_t flags
) ROBOKIT_WL_PACKAGE( 3.4 ) {
	return tc_chain_push_command(chain, timeout_ms, command, flags);
}

robokit_err_t robokit_tc_push_callback(S_T_chain *chain, void (*callback)(void)) {
	S_command command = ROBOKIT_COMMAND_INIT;

	if(ROBOKIT_OK != robokit_make_command_callback(&command, callback))
		return ROBOKIT_ERR_TC_NO_CHAIN_MEM;

	return robokit_tc_push_command(chain, 0, &command, 0);
}


robokit_err_t robokit_tc_discard_list(S_T_chain *chain) {
	if(chain) {
		tc_chain_free(chain);
		return ROBOKIT_OK;
	}
	return ROBOKIT_FAIL;
}

robokit_err_t robokit_tc_push_command_list(S_T_chain *chain, uint8_t flags) ROBOKIT_WL_PACKAGE( 3.4 ) {
	if(chain && chain->flags & TCMD_FLAG_PREPARE) {
		S_command command = ROBOKIT_COMMAND_INIT;
		_S_command_callback *cbcmd = ROBOKIT_CMD_CAST(_S_command_callback *, &command);
		cbcmd->callback = (void *)chain;
		cbcmd->cmd = E_COMMAND_TIMED_LIST;
		cbcmd->mode = E_COMMAND_MODE_CHAIN;
		return robokit_push_command(&command, flags);
	}
	return ROBOKIT_FAIL;
}

robokit_err_t robokit_make_command_callback(S_command *command, void (*callback)(void)) {
	if(!command || !callback)
		return ROBOKIT_FAIL;
	ROBOKIT_COMMAND_RESET_P(command);
	_S_command_callback *cbcmd = ROBOKIT_CMD_CAST(_S_command_callback *, command);
	cbcmd->callback = callback;
	cbcmd->cmd = E_COMMAND_TIMED_LIST;
	cbcmd->mode = E_COMMAND_MODE_CALLBACK;
	return ROBOKIT_OK;
}

ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_TIMED_LIST, S_command *cmd, uint8_t mode, uint8_t *flags) {
	if(mode == E_SCHEDULE_MODE_PRECHECK) {
		if(!cmd) {
			*flags |= 0xFF;
			return;
		}

		_S_command_callback *cbcmd = ROBOKIT_CMD_CAST(_S_command_callback *, cmd);

		switch(cbcmd->mode) {
			case 1:
			case 2:
			if(!cbcmd->callback)
				*flags |= 0xFF;
			break;
			default:
				*flags |= 0xFF;
		}
	}

	if(mode == E_SCHEDULE_MODE_PERFORM) {
		_S_command_callback *cbcmd = ROBOKIT_CMD_CAST(_S_command_callback *, cmd);
		if(cbcmd->mode == 1) {
			void (*callback)(void) = cbcmd->callback;
			if(callback)
				callback();
			return;
		}

		if(cbcmd->mode == 2) {
			// TODO: Chain handler
		}
	}
}