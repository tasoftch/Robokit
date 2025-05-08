//
// Created by Thomas Abplanalp on 11.03.25.
//

#ifndef IMU_H
#define IMU_H

#include <esp_err.h>
#include <values.h>

#define IMU_FILTER_DIR_THRESHOLD 10
#define IMU_FILTER_DIR_TRUST_THRESHOLD 30


typedef struct {
	// Initial position of Imu.
	// During whole uptime, it remains the same
	robokit_degree16_t position;

	// Currently used orientation.
	// It can be fixed using the functions below.
	robokit_degree16_dev_t orientation;

	// The deviation holds the difference to the last measurement
	robokit_degree16_dev_t deviation;
} S_imu_stack_t;

// The interpreter function can be set and enabled.
// Then, each orientation change is notified.
typedef void (*imu_interpreter_fcn_t)(S_imu_stack_t *imu_stack_result);

// Basically start and stop IMU
// Note: If imu is stopped, no values can be obtained
// actual values
void imu_start(void);
void imu_stop(void);

robokit_degree16_t imu_get_position(void);
robokit_degree16_dev_t imu_get_orientation(void);
robokit_degree16_dev_t imu_get_deviation(void);

void imu_fix_orientation(robokit_degree16_t at_position);
void imu_fix_current_orientation(void);

robokit_degree16_t imu_get_fixed_orientation(void);

void imu_set_interpreter(imu_interpreter_fcn_t imu_interpreter_fcn);
void imu_enable_interpreter(void);
void imu_disable_interpreter(void);

#endif //IMU_H
