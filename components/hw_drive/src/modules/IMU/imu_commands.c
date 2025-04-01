//
// Created by Thomas Abplanalp on 11.03.25.
//

#include <imu_commands.h>
#include "hal/pwm_motors.h"
#include <modules/robokit_module.h>
#include <private/robokit_log.h>
#include "hal/imu.h"

typedef struct {
	T_cmd cmd;
	uint8_t flags;
	S_motor_ctl imu_drive_info;
	uint8_t reserved;
	void (*callback)(void);
} _S_imu_cmd;

static S_motor_ctl imu_drive_info = {0};
static S_vector old_drive_vector = {0};

void imu_commands_drive_interpreter(int16_t deviation_angle) {
	uint8_t neg = 0;
	if (deviation_angle < 0) {
		neg = 1;
		deviation_angle = -deviation_angle;
	}

	S_vector drive_vector = {.speed = imu_drive_info.speed, .angle = 0};

	if( deviation_angle > 20 ) {
		drive_vector.angle += neg ? -9 : 9;
	} else if(deviation_angle > 10) {
		drive_vector.angle += neg ? -6 : 6;
	} else if(deviation_angle != 0) {
		drive_vector.angle += neg ? -3 : 3;
	}

	printf("drive_vector.angle = %d, drive_vector.speed = %d\n", drive_vector.angle, drive_vector.speed);

	if(!robokit_vector_equals(old_drive_vector, drive_vector)) {
		S_command command;
		robokit_make_drive_command_vector(&command, drive_vector);
		old_drive_vector = drive_vector;

		robokit_push_command(&command, 0);
	}
}

ROBOKIT_MODULE_INIT() {
	imu_deviation_interpreter(imu_commands_drive_interpreter);
}

ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_IMU, _S_imu_cmd *cmd, uint8_t mode, uint8_t *flags) {
	if(mode == E_SCHEDULE_MODE_PERFORM) {
		imu_set_imu_status( cmd->flags );
		if(cmd->flags & E_IMU_STATUS_FLAG_CALIBRATION) {
			imu_calibration(cmd->callback);
		}

		imu_drive_info = cmd->imu_drive_info;
	}
}

uint8_t robokit_make_command_imu_enable(S_command *command) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_ENABLED;
		return 1;
	}
	return 0;
}

uint8_t robokit_make_command_imu_disable(S_command *command) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_NONE;
		return 1;
	}
	return 0;
}

// The calibration is one cycle only to reset the current orientation to 0.
// Turning the device to the left reduces the angle until 1800° (10 rounds).
// Turning the device to the right increases the angle until 1800° (10 rounds).

uint8_t robokit_make_command_imu_calibration(S_command *command, void (*done_callback)(void)) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_ENABLED | E_IMU_STATUS_FLAG_CALIBRATION;
		cmd->callback = done_callback;
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
		cmd->flags = E_IMU_STATUS_FLAG_ENABLED | E_IMU_STATUS_FLAG_CALIBRATION | E_IMU_STATUS_FLAG_INTERPRET_CALLBACK;
		cmd->imu_drive_info.speed = speed;
		cmd->imu_drive_info.direction = ROBOKIT_DIRECTION_FORWARD;

		return 1;
	}
	return 0;
}
uint8_t robokit_make_command_imu_drive_backward(S_command *command, T_Speed speed) {
	if(command) {
		ROBOKIT_COMMAND_RESET_P(command);
		_S_imu_cmd * cmd = (_S_imu_cmd *)command;
		cmd->cmd = E_COMMAND_IMU;
		cmd->flags = E_IMU_STATUS_FLAG_ENABLED | E_IMU_STATUS_FLAG_CALIBRATION | E_IMU_STATUS_FLAG_INTERPRET_CALLBACK;
		cmd->imu_drive_info.direction = speed;
		cmd->imu_drive_info.direction = ROBOKIT_DIRECTION_BACKWARD;
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