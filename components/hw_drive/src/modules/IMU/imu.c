//
// Created by Thomas Abplanalp on 11.03.25.
//

#include <modules/robokit_module.h>
#include <modules/specs.h>

#include "hal/imu.h"

#include <private/robokit_log.h>

#include "hal/i2c.h"
#include "hal/bno055.h"

#include "values.h"

static uint8_t imu_running = 0;
static uint8_t imu_interpreting = 0;
static uint8_t imu_trigger_stop = 0;
static uint8_t imu_trigger_fix_position = 0;

static imu_interpreter_fcn_t imu_callback;

static S_imu_stack_t imu_stack = {0};
static robokit_degree16_dev_t fixed_orientation = 0;
static volatile robokit_degree16_t old_interpreter_position = INT16_MIN;

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
}

void imu_start(void) {
	imu_running = 1;
}
void imu_stop(void) {
	imu_running = 0;
	if(imu_interpreting) {
		imu_trigger_stop = 1;
	}
	imu_interpreting = 0;
}

robokit_degree16_t imu_get_position(void) { return imu_stack.position; }
robokit_degree16_dev_t imu_get_orientation(void) { return imu_stack.orientation; }
robokit_degree16_dev_t imu_get_deviation(void) { return imu_stack.deviation; }

void imu_fix_orientation(robokit_degree16_t at_position) {
	fixed_orientation = at_position;
	old_interpreter_position = INT16_MIN;
}
robokit_degree16_t imu_get_fixed_orientation(void) {
	return fixed_orientation;
}
void imu_fix_current_orientation(void) {
	imu_trigger_fix_position = 1;
	imu_fix_orientation( imu_stack.position );
}

void imu_set_interpreter(imu_interpreter_fcn_t imu_interpreter_fcn) {
	if(NULL == imu_interpreter_fcn) {
		imu_interpreting = 0;
	}
	imu_callback = imu_interpreter_fcn;
	old_interpreter_position = INT16_MIN;
}
void imu_enable_interpreter(void) {
	if(imu_callback != NULL) {
		imu_interpreting = 1;
		old_interpreter_position = INT16_MIN;
	}
}
void imu_disable_interpreter(void) {
	imu_interpreting = 0;
}

static void imu_interpret(void) {
	if(imu_trigger_fix_position) {
		imu_trigger_fix_position = 0;
		fixed_orientation = imu_stack.position;
	}

	// Calculation to shift orientation to fixed_orientation +/- 180°
	robokit_degree16_dev_t angle = (imu_stack.position - fixed_orientation + 2880) % 5760;
	if (angle < 0) {
		angle += 5760;
	}

	imu_stack.deviation = (angle -2880) - imu_stack.orientation;
	imu_stack.orientation = angle - 2880;

	if(imu_interpreting && imu_callback) {
		if(old_interpreter_position != imu_stack.position) {
			old_interpreter_position = imu_stack.position;
			imu_callback( &imu_stack );
		}
	}
}

/**
 * @brief The sensor loop updates the IMU values if required.
 */
ROBOKIT_MODULE_SENSOR_LOOP() {
	static uint8_t state = 0;

	if(imu_trigger_stop) {
		S_command cmd;
		robokit_make_drive_command_fwd(&cmd, 0);
		robokit_push_command(&cmd, 0);
		imu_trigger_stop = 0;
	}

	if(imu_running) {
		uint8_t buffer[2];

		switch(state) {
			case 0:
				state = 1;
				i2c_read_register_bytes(BNO055_I2C_ADDR, BNO055_VECTOR_EULER, buffer, 2);

				if(buffer[1] > 0x16) {
					// Eventuelle Fehlübertragungen ausschliessen durch Wertebereich.
					// Erwarte Werte zwischen 0 und 5760. (0 - 360°) * 16
					break;
				}


				portMUX_TYPE myMutex = portMUX_INITIALIZER_UNLOCKED;

				taskENTER_CRITICAL(&myMutex);
				imu_stack.position = (buffer[1] << 8) | buffer[0];
				taskEXIT_CRITICAL(&myMutex);
				break;
			case 1:
				state = 2;
				break;
			case 2:
				imu_interpret();

			default:
				state = 0;
		}
	}
}