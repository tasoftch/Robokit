//
// Created by Thomas Abplanalp on 08.04.25.
//

#include "tcp_command_interpreter.h"

#include <device.h>
#include <imu_commands.h>
#include <stdio.h>
#include <string.h>
#include <private/robokit_log.h>
#include <values.h>

enum {
	E_STATUS_READ_DRIVE_VECTOR				= 0xF0,
	E_STATUS_READ_DISTANCE					= 0xF1,
	E_STATUS_READ_FAL_CALIBRATION			= 0xF2,
	E_STATUS_MOTOR_CONFIG					= 0xF3,
	E_STATUS_READ_IMU						= 0xF4,
	E_STATUS_READ_BATTERY_VOLTAGE			= 0xF5,
	E_STATUS_READ_FAL_COLORS				= 0xF6,
	E_STATUS_READ_FAL_COLOR_REPS			= 0xF7,
};

#define _ROBOKIT_MAP_COLOR_TO_BUFFER(POS, COLOR) \
	buffer[POS] = COLOR.red >> 8 & 0xFF; \
	buffer[POS+1] = COLOR.red & 0xFF; \
	buffer[POS+2] = COLOR.green >> 8 & 0xFF; \
	buffer[POS+3] = COLOR.green & 0xFF; \
	buffer[POS+4] = COLOR.blue >> 8 & 0xFF; \
	buffer[POS+5] = COLOR.blue & 0xFF; \


uint8_t tcp_command_interpreter(uint8_t *buffer, uint8_t length) {
	uint8_t cmd = buffer[0];
	if(cmd < ROBOKIT_MAX_SCHEDULED_COMMANDS) {
		if(cmd == E_COMMAND_VECTOR) {
			int8_t angle = buffer[1];
			uint8_t speed = buffer[2];
			S_vector vec = {angle, speed};
			S_command command;

			if(speed < 1) {
				robokit_make_drive_command_fwd(&command, 0);
				robokit_push_command(&command, 0);
				robokit_make_command_imu_disable(&command);
				robokit_push_command(&command, 0);
				robokit_make_command_fal_disable(&command);
				robokit_push_command(&command, 0);
			} else {
				robokit_make_drive_command_vector(&command, vec);
				robokit_push_command(&command, 0);
			}

			return 0;
		}
		if(length >= 8) {
			S_command *command = (S_command *)buffer;
			robokit_push_command(command, 0);
		} else {
			ROBOKIT_LOGE("Invalid command %02X with length %d", cmd, length);
		}
		return 0;
	} else {
		ROBOKIT_LOGI("Status command %02X", cmd);
		switch (cmd) {
			case E_STATUS_MOTOR_CONFIG:
				uint32_t info = 0x257;

			S_motor_config lcfg = robokit_motor_left_get_config();
			S_motor_config rcfg = robokit_motor_right_get_config();
			buffer[0] = 0xF3;
			buffer[1] = *(uint8_t*) &lcfg;
			buffer[2] = *(uint8_t*) &rcfg;
			buffer[3] = 0x0;
			memcpy(buffer+4, &info, sizeof(uint32_t));
			return 8;
			case E_STATUS_READ_IMU:
				int16_t pos = robokit_imu_get_position();
				buffer[0] = pos >> 8 & 0xff;
				buffer[1] = pos & 0xff;

				pos = robokit_imu_get_orientation();
				buffer[2] = pos >> 8 & 0xff;
				buffer[3] = pos & 0xff;

				pos = robokit_imu_get_deviation();
				buffer[4] = pos >> 8 & 0xff;
				buffer[5] = pos & 0xff;
				return 6;
			case E_STATUS_READ_BATTERY_VOLTAGE:
				robokit_voltage_mV_t volts = robokit_battery_get_voltage();
				uint8_t charge = robokit_battery_get_charge_percent();
				uint8_t status = robokit_battery_get_status();

				buffer[0] = volts >> 8 & 0xff;
				buffer[1] = volts & 0xff;
				buffer[2] = charge & 0xff;
				buffer[3] = status & 0xff;
			return 4;
			case E_STATUS_READ_DRIVE_VECTOR:
				S_vector vec = robokit_get_current_vector();
				buffer[0] = vec.angle;
				buffer[1] = vec.speed;
				return 2;
			case E_STATUS_READ_DISTANCE:
				robokit_distance_cm_t dist = robokit_ultrasonic_get_distance();
				buffer[0] = dist>>8&0xFF;
				buffer[1] = dist & 0xff;
				return 2;
			case E_STATUS_READ_FAL_CALIBRATION:
				uint8_t cal = robokit_fal_get_calibration_status();
			buffer[0] = cal & 0xff;
			return 1;
			case E_STATUS_READ_FAL_COLORS:
				buffer[0] = 0x2;
				S_color col = robokit_fal_get_color_left();
				_ROBOKIT_MAP_COLOR_TO_BUFFER(1, col);

			col = robokit_fal_get_color_middle_left();
			_ROBOKIT_MAP_COLOR_TO_BUFFER(7, col);

			col = robokit_fal_get_color_middle();
			_ROBOKIT_MAP_COLOR_TO_BUFFER(13, col);

			col = robokit_fal_get_color_middle_right();
			_ROBOKIT_MAP_COLOR_TO_BUFFER(19, col);

			col = robokit_fal_get_color_right();
			_ROBOKIT_MAP_COLOR_TO_BUFFER(25, col);
			return 31;
			case E_STATUS_READ_FAL_COLOR_REPS:
				S_Fal_Result result = robokit_fal_get_last_result();
				memcpy(buffer, &result, 2);
				if(robokit_fal_is_running()) {
					buffer[1] |= 0x80;
				}
				return 2;
			default:
		}
	}
	return 0xFF;
}