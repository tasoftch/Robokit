//
// Created by Thomas Abplanalp on 08.05.25.
//
#include "values.h"
#include "hal/imu.h"

// Forwarded in drive_command.c
// S_vector robokit_get_current_vector(void)


robokit_degree16_t robokit_imu_get_position(void) {
	return imu_get_position();
}
robokit_degree16_dev_t robokit_imu_get_orientation(void) {
	return imu_get_orientation();
}
robokit_degree16_dev_t robokit_imu_get_deviation(void) {
	return imu_get_deviation();
}