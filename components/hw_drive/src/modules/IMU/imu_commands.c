//
// Created by Thomas Abplanalp on 11.03.25.
//

#include <imu_commands.h>
#include "hal/pwm_motors.h"
#include <modules/robokit_module.h>
#include "hal/imu.h"

enum {
	E_IMU_STATUS_FLAG_DISABLE			= 0,
	E_IMU_STATUS_FLAG_ENABLE			= 1<<0,
	E_IMU_STATUS_FLAG_REFERENCE			= 1<<1,
	E_IMU_STATUS_FLAG_INTERPRET			= 1<<2,
	E_IMU_STATUS_FLAG_STOP				= 1<<3,
	E_IMU_STATUS_FLAG_REF_MOD			= 1<<4
};

typedef struct {
	T_cmd cmd;
	uint8_t flags;
	S_motor_ctl imu_drive_info;
	int8_t deviation;
	void (*callback)(void);
} _S_imu_cmd;

ROBOKIT_CHECK_COMMAND_STRUCT(_S_imu_cmd);


static S_motor_ctl imu_drive_info = {0};
static S_vector old_drive_vector = {0};

void imu_commands_drive_interpreter(const S_imu_stack_t *stack) {
	uint8_t neg = 0;
	robokit_degree16_dev_t deviation_angle = stack->orientation;

	if (deviation_angle < 0) {
		neg = 1;
		deviation_angle = -deviation_angle;
	}

	S_vector drive_vector = {.speed = imu_drive_info.speed, .angle = 0};

	if( deviation_angle > 20 ) {
		drive_vector.angle -= neg ? -9 : 9;
	} else if(deviation_angle > 10) {
		drive_vector.angle -= neg ? -6 : 6;
	} else if(deviation_angle != 0) {
		drive_vector.angle -= neg ? -3 : 3;
	}

	if(!robokit_vector_equals(old_drive_vector, drive_vector)) {
		S_command command;
		robokit_make_drive_command_vector(&command, drive_vector);
		old_drive_vector = drive_vector;

		robokit_push_command(&command, 0);
	}
}

ROBOKIT_MODULE_INIT() {
	imu_set_interpreter(imu_commands_drive_interpreter);
}

ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_IMU, _S_imu_cmd *cmd, uint8_t mode, uint8_t *flags) {
	if(mode == E_SCHEDULE_MODE_PERFORM) {
		if(cmd->flags < 1) {
			imu_stop();
		} else {
			if(cmd->flags & E_IMU_STATUS_FLAG_ENABLE) {
				imu_start();
			}
			if(cmd->flags & E_IMU_STATUS_FLAG_REFERENCE) {
				if(cmd->flags & E_IMU_STATUS_FLAG_REF_MOD) {
					if(cmd->deviation) {
						robokit_degree16_dev_t dev = imu_get_fixed_orientation();
						imu_fix_orientation(dev + cmd->deviation);
					}
				} else {
					imu_fix_current_orientation();
				}
			}
			if(cmd->flags & E_IMU_STATUS_FLAG_INTERPRET) {
				imu_enable_interpreter();
				imu_drive_info = cmd->imu_drive_info;
			}
			if(cmd->flags & E_IMU_STATUS_FLAG_STOP) {
				imu_disable_interpreter();
			}
		}
	}
}

uint8_t robokit_make_command_imu_enable(S_command *command) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_ENABLE;
		return 1;
	}
	return 0;
}

uint8_t robokit_make_command_imu_disable(S_command *command) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_DISABLE;
		return 1;
	}
	return 0;
}

// The calibration is one cycle only to reset the current orientation to 0.
// Turning the device to the left reduces the angle until 1800° (10 rounds).
// Turning the device to the right increases the angle until 1800° (10 rounds).

uint8_t robokit_make_command_imu_reference(S_command *command) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_ENABLE | E_IMU_STATUS_FLAG_REFERENCE;
		return 1;
	}
	return 0;
}

// Giving a drive command enables the IMU and applies a calibration.
// The system will now follow 0° and inject commands to correct the angle.
uint8_t robokit_make_command_imu_drive_forward(S_command *command, T_Speed speed) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_ENABLE | E_IMU_STATUS_FLAG_REFERENCE | E_IMU_STATUS_FLAG_INTERPRET;
		cmd->imu_drive_info.speed = speed;
		cmd->imu_drive_info.direction = ROBOKIT_DIRECTION_FORWARD;

		return 1;
	}
	return 0;
}

uint8_t robokit_make_command_imu_deviate(S_command *command, int8_t deviation) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_ENABLE | E_IMU_STATUS_FLAG_REFERENCE | E_IMU_STATUS_FLAG_REF_MOD;
		cmd->deviation = deviation;

		return 1;
	}
	return 0;
}

uint8_t robokit_make_command_imu_stop(S_command *command) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_STOP;
		return 1;
	}
	return 0;
}