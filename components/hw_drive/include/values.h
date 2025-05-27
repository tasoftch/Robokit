/*
 * MIT License
 *
 * Copyright (c) 2024 Th. Abplanalp, F. Romer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef values_h
#define values_h

#include "config.h"
#include "vector.h"
#include "fal_commands.h"


#define ROBOKIT_DIRECTION_FORWARD (1)
#define ROBOKIT_DIRECTION_BACKWARD (0)

const char *robokit_device_get_serial(void);

// Returns the current vector that the drive is running now.
S_vector robokit_get_current_vector(void);

// IMU value
// Only available after imu calibration and imu enable commands

// Returns the current orientation regarding the calibration.
// If IMU is not running, returns INT16_MIN.

// Degree 16 is an internal usage of degrees from 0 to 5760
// To transform into real degrees, divide by 16.
// It represents a circle from 0 to 360 degrees.
// 0 and 5760 is the same angle.
typedef int16_t robokit_degree16_t;

// Degree 16 dev is also an internal usage of degrees from -2880 to 2800
// To transform into real degrees, divide by 16.
// It represents also a circle from -180 to +180
// -2880 and +2880 is the same angle (opposite of "in front of")
typedef int16_t robokit_degree16_dev_t;

robokit_degree16_t robokit_imu_get_position(void);
robokit_degree16_dev_t robokit_imu_get_orientation(void);
robokit_degree16_dev_t robokit_imu_get_deviation(void);


typedef uint16_t robokit_voltage_mV_t;

enum {
	ROBOKIT_BATTERY_STATUS_EMERGENCY,
	ROBOKIT_BATTERY_STATUS_CRITICAL,
	ROBOKIT_BATTERY_STATUS_WARNING,
	ROBOKIT_BATTERY_STATUS_OK
};

robokit_voltage_mV_t robokit_battery_get_voltage(void);
uint8_t robokit_battery_get_charge_percent(void);
uint8_t robokit_battery_get_status(void);


typedef uint16_t robokit_distance_cm_t;
robokit_distance_cm_t robokit_ultrasonic_get_distance(void);

enum {
	E_CALIBRATION_STATUS_UNCALIBRATED = 0,
	E_CALIBRATION_STATUS_CALIBRATING,
	E_CALIBRATION_STATUS_CALIBRATION_DONE,
	E_CALIBRATION_STATUS_CALIBRATION_FAILED = 0xFF
};

typedef struct {
	uint16_t red;
	uint16_t green;
	uint16_t blue;
} S_color;

uint8_t robokit_fal_get_calibration_status(void);
/**
 * @brief Checks if the calibration process is completed.
 *
 * This function returns the current calibration status of the system,
 * indicating whether the calibration has been successfully finished or not.
 *
 * @return uint8_t Non-zero if calibrated, zero if not calibrated.
 */
uint8_t robokit_fal_is_calibrated(void);

uint8_t robokit_fal_is_running(void);

uint8_t robokit_fal_is_one_shot_complete(void);
S_color robokit_fal_get_color_left(void);
S_color robokit_fal_get_color_middle_left(void);
S_color robokit_fal_get_color_middle(void);
S_color robokit_fal_get_color_middle_right(void);
S_color robokit_fal_get_color_right(void);

S_Fal_Result robokit_fal_get_last_result(void);

S_color robokit_fal_get_calibrated_threshold_color_left(void);
S_color robokit_fal_get_calibrated_threshold_color_middle_left(void);
S_color robokit_fal_get_calibrated_threshold_color_middle(void);
S_color robokit_fal_get_calibrated_threshold_color_middle_right(void);
S_color robokit_fal_get_calibrated_threshold_color_right(void);

#endif /* values_h */
