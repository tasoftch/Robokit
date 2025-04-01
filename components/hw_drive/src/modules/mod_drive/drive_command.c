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

#include "drive_command.h"

#include <portmacro.h>
#include <hal/pwm_motors.h>
#include <modules/robokit_module.h>
#include <private/motor_logic.h>

#include <values.h>

/**
 * @brief Default pwm configuration for left motor
 */
static S_motor_config left_config = {.motor_1 = 0, .motor_2 = 0, .motor_3 = 1, .motor_4 = 0, .fast_decay = 0, .switch_direction = 0};

/**
 * @brief Default pwm configuration for right motor
 */
static S_motor_config right_config = {.motor_1 = 0, .motor_2 = 1, .motor_3 = 0, .motor_4 = 0, .fast_decay = 0, .switch_direction = 0};

/**
 * @inheritDoc
 */
void robokit_motor_left_set_config(const S_motor_config config) { left_config = config; }

/**
 * @inheritDoc
 */
void robokit_motor_right_set_config(const S_motor_config config) { right_config = config; }

/**
 * @inheritDoc
 */
S_motor_config robokit_motor_left_get_config(void) { return left_config; }

/**
 * @inheritDoc
 */
S_motor_config robokit_motor_right_get_config(void) { return right_config; }

static volatile S_vector _current_vector = {0};

/**
 * @brief Helper function to apply motor configuration and motor control into according pwm signals.
 *
 * @param cfg The motor configuration
 * @param ctl The speed information for motor control
 * @internal
 */
static void _robokit_motor_set_speed(const S_motor_config *cfg, const S_motor_ctl *ctl) {
	uint8_t flags = 0;
	flags |= cfg->fast_decay ? E_DRIVE_MOTOR_DR_LOW_FLAG : E_DRIVE_MOTOR_DR_LOW_FLAG|E_DRIVE_MOTOR_DR_HIGH_FLAG;
	flags |= ctl->direction ? E_DRIVE_MOTOR_FORWARD_FLAG : E_DRIVE_MOTOR_BACKWARD_FLAG;

	if(cfg->switch_direction)
		flags |= E_DRIVE_MOTOR_SWITCH_CONNECTORS;

	if(cfg->motor_1) {
		robokit_pwm_motor_1_speed(ctl->speed, flags);
	}

	if(cfg->motor_2) {
		robokit_pwm_motor_2_speed(ctl->speed, flags);
	}

	if(cfg->motor_3) {
		robokit_pwm_motor_3_speed(ctl->speed, flags);
	}

	if(cfg->motor_4) {
		robokit_pwm_motor_4_speed(ctl->speed, flags);
	}
}

/**
 * @brief Main command handler for all drive commands.
 *
 * The handler will accept all commands according to the flags themselves.
 * If the perform mode is selectec, it will forward the command to the motor pwm configuration.
 */
ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_VECTOR, _S_command_drive *cmd, uint8_t mode, uint8_t *flags) {
	if(mode == E_SCHEDULE_MODE_PRECHECK) {
		*flags = cmd->flags;
	}

	if(mode == E_SCHEDULE_MODE_PERFORM) {
		portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
		portENTER_CRITICAL(&myMutex);
		_current_vector.angle = cmd->angle;
		_current_vector.speed = cmd->speed;
		portEXIT_CRITICAL(&myMutex);

		if(cmd->speed == 0) {
			robokit_pwm_motor_all_off();
			return;
		}

		_robokit_motor_set_speed(&cmd->m_left, &cmd->m_ctl_left);
		_robokit_motor_set_speed(&cmd->m_right, &cmd->m_ctl_right);
	}
}


/**
 * @inheritDoc
 */
uint8_t robokit_make_drive_command_fwd(S_command *cmd, const T_Speed speed) {
	if( robokit_make_drive_command_vector(cmd, robokit_make_vector_polar(0, speed) ) ) {
		return 1;
	}
	return 0;
}

/**
 * @inheritDoc
 */
uint8_t robokit_make_drive_command_bwd(S_command *cmd, const T_Speed speed) {
	if( robokit_make_drive_command_vector(cmd, robokit_make_vector_polar(180, speed) ) ) {
		return 1;
	}
	return 0;
}


/**
 * @inheritDoc
 */
uint8_t robokit_make_drive_command_vector(S_command *cmd, const S_vector vector) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);
	_S_command_drive *drive_cmd = ROBOKIT_CMD_CAST(_S_command_drive*, cmd);
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

/**
 *
 * @return S_vector The current vector
 */
S_vector robokit_get_current_vector(void) {
	return _current_vector;
}