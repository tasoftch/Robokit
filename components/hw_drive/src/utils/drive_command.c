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

#include <motor_logic.h>
#include <scheduler.h>
#include <pwm_motors.h>

#include "command_data_containers.h"
#include "robokit_log.h"

extern void _robokit_command_init(S_command *cmd);

static S_motor_config left_config = {.motor_1 = 0, .motor_2 = 0, .motor_3 = 1, .motor_4 = 0, .low_drive = 0, .switch_direction = 0};
static S_motor_config right_config = {.motor_1 = 0, .motor_2 = 0, .motor_3 = 0, .motor_4 = 1, .low_drive = 0, .switch_direction = 0};

void robokit_motor_left_set_config(S_motor_config config) { left_config = config; }
void robokit_motor_right_set_config(S_motor_config config) { right_config = config; }

S_motor_config robokit_motor_left_get_config(void) { return left_config; }
S_motor_config robokit_motor_right_get_config(void) { return right_config; }

static void _robokit_task_handler(_S_command_drive *cmd, uint8_t mode, uint8_t *flags) {
	if(mode == E_SCHEDULE_MODE_PRECHECK) {
		*flags = cmd->flags;
	}

	if(mode == E_SCHEDULE_MODE_PERFORM) {
		if(cmd->speed == 0) {
			robokit_pwm_motor_all_off();
			return;
		}

		uint8_t flags = 0;
		flags |= cmd->m_left.low_drive ? E_DRIVE_MOTOR_DR_LOW_FLAG : (E_DRIVE_MOTOR_DR_LOW_FLAG|E_DRIVE_MOTOR_DR_HIGH_FLAG);
		flags |= cmd->m_ctl_left.direction ? E_DRIVE_MOTOR_FORWARD_FLAG : E_DRIVE_MOTOR_BACKWARD_FLAG;

		if(cmd->m_left.switch_direction)
			flags |= E_DRIVE_MOTOR_SWITCH_CONNECTORS;

		if(cmd->m_left.motor_1) {
			robokit_pwm_motor_1_speed(cmd->m_ctl_left.speed, flags);
		}

		if(cmd->m_left.motor_2) {
			robokit_pwm_motor_2_speed(cmd->m_ctl_left.speed, flags);
		}

		if(cmd->m_left.motor_3) {
			robokit_pwm_motor_3_speed(cmd->m_ctl_left.speed, flags);
		}

		if(cmd->m_left.motor_4) {
			robokit_pwm_motor_4_speed(cmd->m_ctl_left.speed, flags);
		}

		flags = 0;
		flags |= cmd->m_right.low_drive ? E_DRIVE_MOTOR_DR_LOW_FLAG : (E_DRIVE_MOTOR_DR_LOW_FLAG|E_DRIVE_MOTOR_DR_HIGH_FLAG);
		flags |= cmd->m_ctl_right.direction ? E_DRIVE_MOTOR_FORWARD_FLAG : E_DRIVE_MOTOR_BACKWARD_FLAG;

		if(cmd->m_right.switch_direction)
			flags |= E_DRIVE_MOTOR_SWITCH_CONNECTORS;

		if(cmd->m_right.motor_1) {
			robokit_pwm_motor_1_speed(cmd->m_ctl_right.speed, flags);
		}

		if(cmd->m_right.motor_2) {
			robokit_pwm_motor_2_speed(cmd->m_ctl_right.speed, flags);
		}

		if(cmd->m_right.motor_3) {
			robokit_pwm_motor_3_speed(cmd->m_ctl_right.speed, flags);
		}

		if(cmd->m_right.motor_4) {
			robokit_pwm_motor_4_speed(cmd->m_ctl_right.speed, flags);
		}
	}
}


void _drive_commands_init(void) {
	ROBOKIT_LOGI("drive_commands_init");
	robokit_register_command_fn(E_COMMAND_VECTOR, _robokit_task_handler);
}

uint8_t robokit_make_drive_command_fwd(S_command *cmd, T_Speed speed) {
	if( robokit_make_drive_command_vector(cmd, robokit_make_vector_polar(0, speed) ) ) {
		return robokit_drive_command_enable_imu(cmd);
	}
	return 0;
}

uint8_t robokit_make_drive_command_bwd(S_command *cmd, T_Speed speed) {
	if( robokit_make_drive_command_vector(cmd, robokit_make_vector_polar(180, speed) ) ) {
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
	_S_command_drive *drive_cmd = _ROBOKIT_CMD_CAST(_S_command_drive*, cmd);
	drive_cmd->command = E_COMMAND_VECTOR;
	drive_cmd->angle = vector.angle;
	drive_cmd->speed = vector.speed;

	if(robokit_vector_to_motor(vector, &drive_cmd->m_ctl_left, &drive_cmd->m_ctl_right)) {
		drive_cmd->m_left = robokit_motor_left_get_config();
		drive_cmd->m_right = robokit_motor_right_get_config();
		return 1;
	}
	return 0;
}