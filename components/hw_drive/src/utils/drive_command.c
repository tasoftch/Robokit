// Copyright (c) 2024 Th. Abplanalp
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the “Software”), to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

//
// Created by Thomas Abplanalp on 15.11.24.
//

#include "drive_command.h"
#include "command_data_containers.h"

extern void _robokit_command_init(S_command *cmd);

void _drive_commands_init(void) {
	printf("CMD Size %d\n", sizeof(_S_command_drive));
}

uint8_t robokit_make_drive_command_fwd(S_command *cmd, T_Speed speed) {
	if( robokit_make_drive_command_vector(cmd, ROBOKIT_MAKE_VECTOR(0, speed) ) ) {
		return robokit_drive_command_enable_imu(cmd);
	}
	return 0;
}

uint8_t robokit_make_drive_command_bwd(S_command *cmd, T_Speed speed) {
	if( robokit_make_drive_command_vector(cmd, ROBOKIT_MAKE_VECTOR(180, speed) ) ) {
		return robokit_drive_command_enable_imu(cmd);
	}
	return 0;
}

uint8_t robokit_drive_command_enable_imu(S_command *cmd) {
	if(!cmd)
		return 0;
	_ROBOKIT_CMD_CAST(_S_command_drive *, cmd)->flags |= E_COMMAND_FLAG_IMU;
	return 1;
}

uint8_t robokit_drive_command_disable_imu(S_command *cmd) {
	if(!cmd)
		return 0;
	_ROBOKIT_CMD_CAST(_S_command_drive *, cmd)->flags &= ~E_COMMAND_FLAG_IMU;
	return 1;
}

uint8_t robokit_make_drive_command_vector(S_command *cmd, S_vector vector) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	cmd->cmd = E_COMMAND_VECTOR;
	_ROBOKIT_CMD_CAST(_S_command_drive *, cmd)->angle = vector.angle;
	_ROBOKIT_CMD_CAST(_S_command_drive *, cmd)->speed = vector.speed;
	return 1;
}