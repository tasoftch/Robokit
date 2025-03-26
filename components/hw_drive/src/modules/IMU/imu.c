//
// Created by Thomas Abplanalp on 11.03.25.
//

#include <modules/robokit_module.h>
#include <modules/specs.h>

#include "hal/imu.h"

#include <private/robokit_log.h>

#include "hal/i2c.h"
#include "hal/bno055.h"
#include "private/filter.h"

static S_filter direction_filter;
static uint8_t imu_status = 0;
static void (*imu_callback_calibration)(void);
static uint8_t imu_calibration_count = 0;

static void imu_interpret(void) ROBOKIT_SPECIFICATION( SC01_0 ) {
	if (imu_status & E_IMU_STATUS_FLAG_CALIBRATION) {
		if(imu_calibration_count-- < 1) {
			imu_status &= ~E_IMU_STATUS_FLAG_CALIBRATION;
			filter_angle_put_reference(&direction_filter);
			if(imu_callback_calibration)
				imu_callback_calibration();
			imu_callback_calibration = NULL;
			ROBOKIT_LOGI("Calibration complete at %d", filter_angle_get_reference(&direction_filter));
		}
	}

	int16_t dir = filter_angle_get_value(&direction_filter);

	ROBOKIT_LOGI("IMU DIR: %d", dir);
}

void imu_calibration(void(*callback)(void)) {
	imu_callback_calibration = callback;
	imu_status |= E_IMU_STATUS_FLAG_CALIBRATION;
	imu_calibration_count = 30;
}

ROBOKIT_MODULE_INIT() {
	i2c_init();

	bno055_setup();
	bno055_setOperationModeNDOF();

	imu_status = 1;
	filter_angle_init(&direction_filter, 10, 30);

	imu_calibration(NULL);
}

ROBOKIT_MODULE_SENSOR_LOOP() {
	static uint8_t state = 0;
	if(imu_status & 1) {
		uint8_t buffer[2];

		switch(state) {
			case 0:
				i2c_read_register_bytes(BNO055_I2C_ADDR, BNO055_VECTOR_EULER, buffer, 2);
				filter_angle_put_value(&direction_filter, (buffer[1] << 8) | buffer[0]);
				state = 1;
				break;
			case 1:
				i2c_read_register_bytes(BNO055_I2C_ADDR, BNO055_VECTOR_LINEARACCEL, buffer, 2);
				// imu_add_acceleration((buffer[1] << 8) | buffer[0]);
				state = 2;
				break;
			case 2:
				imu_interpret();

			default:
				state = 0;
		}
	}
}