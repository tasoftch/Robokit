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

#include "fal_commands.h"
#include <hal/fal.h>
#include <modules/robokit_module.h>

ROBOKIT_CHECK_COMMAND_STRUCT(_S_Command_Fal);

/**
 * @inheritDoc
 */
uint8_t robokit_make_command_fal_drive(S_command *cmd, T_Speed speed, uint16_t timeout) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);
	_S_Command_Fal *fal_cmd = ROBOKIT_CMD_CAST(_S_Command_Fal *, cmd);
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_ENABLE_DRIVE;
	fal_cmd->speed = speed;
	fal_cmd->timeout = timeout / 40;

	return 1;
}

/**
 * @inheritDoc
 */
uint8_t robokit_make_command_fal_enable(S_command *cmd) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);
	_S_Command_Fal *fal_cmd = ROBOKIT_CMD_CAST(_S_Command_Fal *, cmd);
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_ENABLE;

	return 1;
}

/**
 * @inheritDoc
 */
uint8_t robokit_make_command_fal_disable(S_command *cmd) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);
	_S_Command_Fal *fal_cmd = ROBOKIT_CMD_CAST(_S_Command_Fal *, cmd);
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_DISABLE;

	return 1;
}

/**
 * @inheritDoc
 */
uint8_t robokit_make_command_fal_calibrate(S_command *cmd, T_Speed speed, uint16_t timeout, void (*calibrated)(uint8_t)) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);
	_S_Command_Fal *fal_cmd = ROBOKIT_CMD_CAST(_S_Command_Fal *, cmd);
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_CALIBRATE;
	fal_cmd->callback = calibrated;
	fal_cmd->speed = speed;
	if(timeout > 10000)
		timeout = 10000;
	fal_cmd->timeout = timeout / 40;
	return 1;
}

uint8_t robokit_make_command_fal_one_shot(S_command *cmd, void(*complete_handler)(void)) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);
	_S_Command_Fal *fal_cmd = ROBOKIT_CMD_CAST(_S_Command_Fal *, cmd);
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_SHOT;
	fal_cmd->callback = complete_handler;
	return 1;
}