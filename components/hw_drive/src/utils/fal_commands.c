//
// Created by Thomas Abplanalp on 27.11.24.
//

#include "fal_commands.h"
#include <command_data_containers.h>

extern void _robokit_command_init(S_command *cmd);


uint8_t robokit_make_command_fal_enable(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	_S_Command_Fal *fal_cmd = (_S_Command_Fal *)cmd;
	fal_cmd->cmd = E_COMMAND_FAL;

	return 1;
}

uint8_t robokit_make_command_fal_disable(S_command *cmd) {

	return 0;
}