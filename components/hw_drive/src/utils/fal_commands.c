//
// Created by Thomas Abplanalp on 27.11.24.
//

#include "fal_commands.h"
#include <command_data_containers.h>
#include "fal.h"

extern void _robokit_command_init(S_command *cmd);


uint8_t robokit_make_command_fal_enable(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	_S_Command_Fal *fal_cmd = (_S_Command_Fal *)cmd;
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_ENABLE;

	return 1;
}

uint8_t robokit_make_command_fal_disable(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	_S_Command_Fal *fal_cmd = (_S_Command_Fal *)cmd;
	fal_cmd->cmd = E_COMMAND_FAL;
	fal_cmd->flags = E_FAL_OPTION_DISABLE;

	return 1;
}

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