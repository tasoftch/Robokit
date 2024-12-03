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

#include <motor_logic.h>
#include <scheduler.h>
#include <pwm_motors.h>

#include "command_data_containers.h"
#include "robokit_log.h"

extern void _robokit_command_init(S_command *cmd);

static S_motor_config left_config = {.motor_1 = 0, .motor_2 = 0, .motor_3 = 1, .motor_4 = 0, .fast_decay = 0, .switch_direction = 0};
static S_motor_config right_config = {.motor_1 = 0, .motor_2 = 0, .motor_3 = 0, .motor_4 = 1, .fast_decay = 0, .switch_direction = 0};

void robokit_motor_left_set_config(S_motor_config config) { left_config = config; }
void robokit_motor_right_set_config(S_motor_config config) { right_config = config; }

S_motor_config robokit_motor_left_get_config(void) { return left_config; }
S_motor_config robokit_motor_right_get_config(void) { return right_config; }

/**
 * Handles the robokit task based on the given command parameters and mode.
 *
 * This function performs different actions based on the mode. If the mode is
 * E_SCHEDULE_MODE_PRECHECK, it sets the flags output parameter to the flags
 * value in the command. If the mode is E_SCHEDULE_MODE_PERFORM, it evaluates
 * the speed and motor configurations to control the PWM motor speeds.
 *
 * @param cmd A pointer to a structure containing the driving command details,
 * including motor configuration and control parameters.
 * @param mode The operation mode in which the task handler should execute,
 * such as E_SCHEDULE_MODE_PRECHECK or E_SCHEDULE_MODE_PERFORM.
 * @param flags A pointer to a byte where the flags result is stored when in
 * E_SCHEDULE_MODE_PRECHECK mode. This might be used to set motor state flags
 * for direction and decay handling.
 */
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
		flags |= cmd->m_left.fast_decay ? E_DRIVE_MOTOR_DR_LOW_FLAG : (E_DRIVE_MOTOR_DR_LOW_FLAG|E_DRIVE_MOTOR_DR_HIGH_FLAG);
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
		flags |= cmd->m_right.fast_decay ? E_DRIVE_MOTOR_DR_LOW_FLAG : (E_DRIVE_MOTOR_DR_LOW_FLAG|E_DRIVE_MOTOR_DR_HIGH_FLAG);
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


/**
 * Initializes the drive command system and registers the task handler.
 *
 * This function logs the initialization process of drive commands and
 * registers the task handler function _robokit_task_handler with the
 * E_COMMAND_VECTOR, allowing the system to handle and process drive commands
 * accordingly.
 */
void _drive_commands_init(void) {
	ROBOKIT_LOGI("drive_commands_init");
	robokit_register_command_fn(E_COMMAND_VECTOR, _robokit_task_handler);
}

/**
 * Constructs and sends a drive command for moving forward with a specified speed.
 *
 * This function creates a drive command vector with a polar vector at zero
 * angle and a specified speed. If the command vector is successfully
 * created, the IMU (Inertial Measurement Unit) is enabled for the command.
 * The function returns the result of enabling the IMU or zero if the
 * command vector creation fails.
 *
 * @param cmd A pointer to an S_command structure where the drive command
 * will be stored.
 * @param speed A T_Speed value indicating the desired speed of the forward
 * drive.
 * @return A uint8_t indicating the success of enabling IMU for the command;
 * returns a non-zero value if IMU is enabled successfully, otherwise returns
 * 0 if the command vector creation fails.
 */
uint8_t robokit_make_drive_command_fwd(S_command *cmd, T_Speed speed) {
	if( robokit_make_drive_command_vector(cmd, robokit_make_vector_polar(0, speed) ) ) {
		return robokit_drive_command_enable_imu(cmd);
	}
	return 0;
}

/**
 * Constructs a backward drive command with the specified speed and command structure.
 *
 * This function creates a drive command in the backward direction by utilizing
 * a polar vector with an angle of 180 degrees and the given speed. If successful,
 * it enables the IMU (Inertial Measurement Unit) for the command.
 *
 * @param cmd A pointer to a command structure where the backward drive
 * command will be generated.
 * @param speed The speed value to be applied to the backward command.
 *
 * @return Returns a non-zero value if the IMU enabling was successful;
 * returns 0 otherwise.
 */
uint8_t robokit_make_drive_command_bwd(S_command *cmd, T_Speed speed) {
	if( robokit_make_drive_command_vector(cmd, robokit_make_vector_polar(180, speed) ) ) {
		return robokit_drive_command_enable_imu(cmd);
	}
	return 0;
}

/**
 * Enables the IMU (Inertial Measurement Unit) functionality in the drive command.
 *
 * This function modifies the command to enable the IMU by setting the appropriate
 * flag in the command's flags field. It ensures that the IMU is active for the
 * specified command execution.
 *
 * @param cmd A pointer to the command structure to be modified. It should be
 *            a valid command structure where the IMU functionality can be enabled.
 * @return Returns 1 if the IMU flag was successfully set. Returns 0 if the
 *         provided command structure pointer is null.
 */
uint8_t robokit_drive_command_enable_imu(S_command *cmd) {
	if(!cmd)
		return 0;
	_ROBOKIT_CMD_CAST(_S_command_drive *, cmd)->flags |= E_COMMAND_FLAG_IMU;
	return 1;
}

/**
 * Disables the IMU flag in the given driving command.
 *
 * This function clears the E_COMMAND_FLAG_IMU bit in the flags of the provided
 * S_command structure, effectively disabling any IMU (Inertial Measurement Unit)
 * related functionality associated with the command.
 *
 * @param cmd A pointer to an S_command structure representing the driving command
 * whose IMU flag needs to be disabled. If this pointer is null, the function
 * will return 0.
 * @return Returns 1 if the IMU flag was successfully disabled, or 0 if the command
 * pointer was null.
 */
uint8_t robokit_drive_command_disable_imu(S_command *cmd) {
	if(!cmd)
		return 0;
	_ROBOKIT_CMD_CAST(_S_command_drive *, cmd)->flags &= ~E_COMMAND_FLAG_IMU;
	return 1;
}

/**
 * Constructs a drive command vector for the robokit system and initializes the
 * provided command with the specified vector parameters.
 *
 * This function sets up a drive command by initializing the command structure,
 * casting it to a specific drive command type, and then associating it with
 * a drive vector. It converts the vector into motor control parameters and
 * assigns motor configurations if successful.
 *
 * @param cmd A pointer to an S_command structure where the drive command will be
 * initialized and stored. This must not be null.
 * @param vector The S_vector structure containing angle and speed details that
 * define the desired drive command vector.
 * @return Returns 1 if the drive command vector is successfully constructed and
 * configured, and 0 if the command pointer is null or vector conversion failed.
 */
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