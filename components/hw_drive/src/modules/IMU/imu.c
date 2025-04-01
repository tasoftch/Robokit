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

#include "values.h"

static S_filter direction_filter;
static uint8_t imu_status = 0;
static void (*imu_callback_calibration)(void);
static uint8_t imu_calibration_count = 0;


static volatile int16_t current_raw_angle = 0;
static volatile int16_t current_filtered_angle = 0;

static IMU_Interpreter_Callback imu_callback;

/**
 * @inheritDoc
 */
int16_t robokit_get_current_orientation(void) {
	return current_filtered_angle;
}

/**
 * @brief internal interpreter function to handle changes of the orientation
 *
 * @return void
 */
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
		return;
	}

	int16_t new_angle = filter_angle_get_value(&direction_filter);

	if(new_angle != current_raw_angle) {
		portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;
		portENTER_CRITICAL(&myMutex);
		current_filtered_angle = new_angle;
		portEXIT_CRITICAL(&myMutex);

		if(imu_status & E_IMU_STATUS_FLAG_INTERPRET_CALLBACK) {
			if(imu_callback) {
				imu_callback( new_angle );
			}
		}
	}
}

/**
 * @inheritDoc
 */
void imu_set_imu_status(E_imu_status status) {
	imu_status = status;
}

/**
 * @inheritDoc
 */
void imu_deviation_interpreter(IMU_Interpreter_Callback callback) {
	imu_callback = callback;
}

/**
 * @inheritDoc
 */
void imu_calibration(void(*callback)(void)) {
	imu_callback_calibration = callback;
	imu_status |= E_IMU_STATUS_FLAG_CALIBRATION;
	imu_calibration_count = 1;
}

/**
 * @brief Module initialisation.
 *
 * Starts I2C and reboots the BNO055 chip (IMU)
 * The device is oriented as 0 when startup.
 */
ROBOKIT_MODULE_INIT() {
	i2c_init();

	bno055_setup();
	bno055_setOperationModeNDOF();

	filter_angle_init(&direction_filter, 50, 10);
	filter_angle_put_reference(&direction_filter);

	imu_calibration(NULL);
}

/**
 * @brief The sensor loop updates the IMU values if required.
 */
ROBOKIT_MODULE_SENSOR_LOOP() {
	static uint8_t state = 0;

	if(imu_status & E_IMU_STATUS_FLAG_STOP) {
		S_command cmd;
		robokit_make_drive_command_fwd(&cmd, 0);
		robokit_push_command(&cmd, 0);
		imu_status = 0;
	}

	if(imu_status & 1) {
		uint8_t buffer[2];
		portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;

		switch(state) {
			case 0:
				state = 1;
				i2c_read_register_bytes(BNO055_I2C_ADDR, BNO055_VECTOR_EULER, buffer, 2);
				if(buffer[1] > 0x16) {
					// Eventuelle Fehlübertragungen ausschliessen durch Wertebereich.
					// Erwarte Werte zwischen 0 und 5760. (0 - 360°) * 16
					break;
				}

				taskENTER_CRITICAL(&myMutex);
				current_raw_angle = (buffer[1] << 8) | buffer[0];
				taskEXIT_CRITICAL(&myMutex);

				filter_angle_put_value(&direction_filter, current_raw_angle);
				break;
			case 1:
				// Optional: implementation to determine position of device.
				// i2c_read_register_bytes(BNO055_I2C_ADDR, BNO055_VECTOR_LINEARACCEL, buffer, 2);
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