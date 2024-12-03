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
#include <command_data_containers.h>
#include "fal.h"

extern void _robokit_command_init(S_command *cmd);


/**
 * Initializes a command structure to enable the follow a line (FAL) feature.
 *
 * This function sets up the provided command structure to execute the follow
 * a line command with the enable option. The command and flags fields are set
 * to predefined values related to the FAL enable option.
 *
 * @param cmd Pointer to a command structure that will be initialized. It must
 *            be non-null for the operation to succeed.
 * @return Returns 1 if the operation is successful, or 0 if the provided
 *         command structure pointer is null.
 */
uint8_t robokit_make_command_fal_enable(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	_S_Command_Fal *fal_cmd = (_S_Command_Fal *)cmd;
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_ENABLE;

	return 1;
}

/**
 * Initializes a command structure to disable the follow a line (FAL) feature.
 *
 * This function configures the provided command structure to execute the follow
 * a line command with the disable option. The command is set to a predefined FAL
 * command value, and the flags field is adjusted to denote the disable option.
 *
 * @param cmd Pointer to a command structure that will be initialized. This pointer
 *            must be non-null for the function to operate correctly.
 * @return Returns 1 if the structure is successfully initialized, or 0 if the provided
 *         command structure pointer is null.
 */
uint8_t robokit_make_command_fal_disable(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	_S_Command_Fal *fal_cmd = (_S_Command_Fal *)cmd;
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_DISABLE;

	return 1;
}

/**
 * Initializes a command structure to execute the follow a line (FAL) calibration feature.
 *
 * This function configures the given command structure to perform the follow
 * a line calibration command. It sets the command and flags fields to predefined
 * values associated with the calibration option and assigns a callback function
 * to be invoked upon completion of the calibration.
 *
 * @param cmd Pointer to a command structure to be initialized. It must be non-null
 *            for the operation to proceed successfully.
 * @param calibrated Function pointer representing the callback to be executed
 *                   when calibration completes. This function should accept a
 *                   uint8_t parameter.
 * @return Returns 1 if the operation is successful, or 0 if the provided command
 *         structure pointer is null.
 */
uint8_t robokit_make_command_fal_calibrate(S_command *cmd, void (*calibrated)(uint8_t)) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	_S_Command_Fal *fal_cmd = (_S_Command_Fal *)cmd;
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_CALIBRATE;
	fal_cmd->callback = calibrated;

	return 1;
}