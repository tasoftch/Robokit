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

// Voltage measurements are directly in voltage_sensor.c

// Distance getters forwarded to ultrasonic_commands.c

// FAL values forwarded to fal.c

extern const char *device_serial(void);

const char *robokit_device_get_serial(void) {
	return device_serial();
}