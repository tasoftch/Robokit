//
// Created by Thomas Abplanalp on 11.03.25.
//

#include <modules/robokit_module.h>
#include "hal/imu.h"

#include <private/robokit_log.h>

#include "hal/i2c.h"
#include "hal/bno055.h"
#include "private/filter.h"

static S_filter direction_filter;

static int16_t old_direction = 0;
static int16_t reference_direction = 0;
static uint8_t imu_status = 0;

static void imu_add_direction_data(int16_t direction) {
	static uint8_t trust = 0;

	int16_t diff = old_direction - direction;
	if ((diff < 0 ? -diff : diff) < IMU_FILTER_DIR_THRESHOLD) {
		old_direction = direction;
		if(imu_status & E_IMU_STATUS_FLAG_CALIBRATION) {
			imu_status &= ~E_IMU_STATUS_FLAG_CALIBRATION;
			reference_direction = direction;
		}
		trust = 0;
	} else {
		trust++;
		if(trust >= IMU_FILTER_DIR_TRUST_THRESHOLD) {
			old_direction = direction;
			if(imu_status & E_IMU_STATUS_FLAG_CALIBRATION) {
				imu_status &= ~E_IMU_STATUS_FLAG_CALIBRATION;
				reference_direction = direction;
			}
			trust = 0;
		}
	}
}

static void imu_add_acceleration(int16_t acceleration) {

}

static void imu_interpret(void) {
	int16_t dir = (old_direction - reference_direction + 2880) % 5760;
	if (dir < 0) {
		dir += 5760;
	}
	dir -= 2880;

	ROBOKIT_LOGI("IMU DIR: %d", dir);
}

ROBOKIT_MODULE_INIT() {
	i2c_init();

	bno055_setup();
	bno055_setOperationModeNDOF();

	imu_status = 3;
	filter_angle_init(&direction_filter, 10, 30);
}

ROBOKIT_MODULE_SENSOR_LOOP() {
	static uint8_t state = 0;
	if(imu_status & 1) {
		uint8_t buffer[2];

		switch(state) {
			case 0:
				i2c_read_register_bytes(BNO055_I2C_ADDR, BNO055_VECTOR_EULER, buffer, 2);
				imu_add_direction_data((buffer[1] << 8) | buffer[0]);
				state = 1;
				break;
			case 1:
				i2c_read_register_bytes(BNO055_I2C_ADDR, BNO055_VECTOR_LINEARACCEL, buffer, 2);
				imu_add_acceleration((buffer[1] << 8) | buffer[0]);
				state = 2;
				break;
			case 2:
				imu_interpret();

			default:
				state = 0;
		}
	}
}