//
// Created by Thomas Abplanalp on 11.03.25.
//

#ifndef IMU_H
#define IMU_H

#include <esp_err.h>

#define IMU_FILTER_DIR_THRESHOLD 10
#define IMU_FILTER_DIR_TRUST_THRESHOLD 30

/**
 * @brief internal description of the command meaning
 */
typedef enum {
	// Nothing, disable IMU if running
	E_IMU_STATUS_FLAG_NONE = 0,
	// Enable IMU, it will provide orientation values
	E_IMU_STATUS_FLAG_ENABLED = 1<<0,
	// Reset orientation to current position
	E_IMU_STATUS_FLAG_CALIBRATION = 1<<1,
	// Contact a provided callback each time, the orientation changes
	E_IMU_STATUS_FLAG_INTERPRET_CALLBACK = 1<<2,
	// Stop any action of IMU.
	// This also pushes a stop command, if the drive is running under IMU
	E_IMU_STATUS_FLAG_STOP = 1<<3,
} E_imu_status;

/**
 * @brief Interpreter function to transform IMU orientation into drive commands
 */
typedef void (*IMU_Interpreter_Callback)(int16_t deviation_angle);

/**
 * @brief Sets the IMU current status flags to enable/disable features
 *
 * @param status E_imu_status The Status to be set in IMU Firmware
 */
void imu_set_imu_status(E_imu_status status);

/**
 * @brief Sets the current IMU orientation interpreter
 */
void imu_deviation_interpreter(IMU_Interpreter_Callback callback);

/**
 * @brief Starts the calibration to reset the current orientation to 0
 *
 * @param callback A callback that gets contacted when done.
 */
void imu_calibration(void(*callback)(void));

/**
 * @brief Stops any action of IMU including pushing a drive stop command.
 */
void imu_stop(void);

#endif //IMU_H
