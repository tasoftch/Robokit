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

#ifndef DRIVE_COMMAND_H
#define DRIVE_COMMAND_H

#include "main_commands.h"
#include "vector.h"

/**
 * @brief Constructs a drive command for moving forward with a specified speed.
 *
 * This function creates a drive command vector with a polar vector at zero
 * angle and a specified speed.
 *
 * @param cmd A pointer to an S_command structure where the drive command
 * will be stored.
 * @param speed A T_Speed value indicating the desired speed of the forward
 * drive.
 *
 * @return 1 on success, 0 otherwise
 */
uint8_t robokit_make_drive_command_fwd(S_command *cmd, T_Speed speed);

/**
 * @brief Constructs a drive command for moving backward with a specified speed.
 *
 * This function creates a drive command vector with a polar vector at 180
 * angle and a specified speed.
 *
 * @param cmd A pointer to an S_command structure where the drive command
 * will be stored.
 * @param speed A T_Speed value indicating the desired speed of the forward
 * drive.
 *
 * @return 1 on success, 0 otherwise
 */
uint8_t robokit_make_drive_command_bwd(S_command *cmd, T_Speed speed);

/**
 * @brief Constructs a drive command for moving in any given direction specified by a vector.
 *
 * This function creates a drive command using a specified vector.
 *
 * @param cmd A pointer to an S_command structure where the drive command
 * will be stored.
 * @param vector A S_Vector value indicating the desired direction
 *
 * @return 1 on success, 0 otherwise
 */
uint8_t robokit_make_drive_command_vector(S_command *cmd, S_vector vector);

/**
 * Motor configuration
 * The motor configuration is required to assign motor left and motor right.
 */

typedef struct {
 /**
  * @brief Represents the state of the first motor in a motor configuration setup.
  *
  * The motor_1 member is a bit-field that indicates whether the first motor
  * in a set of motors is active. This can be used to toggle the motor on or off
  * in scenarios where bit manipulation provides efficient state control.
  *
  * @param[in] motor_1 uint8_t:1 Bit indicating the active state of motor 1.
  */
 uint8_t motor_1:1;
 /**
  * @brief Bitfield indicating the state of the second motor.
  *
  * Represents a binary flag indicating whether the second motor is active or inactive.
  */
 uint8_t motor_2:1;
 /**
  * @brief Represents the configuration state of the third motor.
  *
  * The `motor_3` bit field indicates whether the third motor in a set of four
  * motors is activated (1) or deactivated (0). It is part of the motor configuration
  * structure used to control multiple motors simultaneously.
  */
 uint8_t motor_3:1;
 /**
  * @brief Represents the configuration state of the fourth motor.
  *
  * This bit field indicates whether the fourth motor is active in the
  * motor configuration structure. It is used in controlling the state
  * of the motor in the drive system.
  */
 uint8_t motor_4:1;
 /**
  * @brief Fast decay mode flag for motor control configuration.
  *
  * This flag indicates whether the fast decay mode is enabled for a motor in the
  * configuration. Fast decay is a method used in motor drive to stop the current
  * flowing through a motor winding quickly, allowing for more precise control over
  * the motor's stopping and responsiveness.
  *
  * @return uint8_t Returns 1 if fast decay mode is enabled, otherwise returns 0.
  */
 uint8_t fast_decay:1;
 /**
  * @brief Indicates if the motor direction should be switched.
  *
  * This field is used to determine whether the direction of motor connectors
  * should be switched. A value of '1' indicates that the direction should be
  * switched, while a value of '0' means the direction remains unchanged.
  *
  * @param[in] switch_direction uint8_t Direction switch flag for motors.
  */
 uint8_t switch_direction:1;
} S_motor_config;


/**
 * @brief Changes the pwm configuration for the left motor
 * @param config The new configuration
 */
void robokit_motor_left_set_config(S_motor_config config);

/**
 * @brief Changes the pwm configuration for the right motor
 * @param config The new configuration
 */
void robokit_motor_right_set_config(S_motor_config config);

/**
 * @brief Returns the current active pwm configuration for the left motor
 * @return The current configuration
 */
S_motor_config robokit_motor_left_get_config(void);

/**
 * @brief Returns the current active pwm configuration for the right motor
 * @return The current configuration
 */
S_motor_config robokit_motor_right_get_config(void);

/**
 * @brief Enables the IMU flag in the drive command.
 * @param [out] cmd S_command* Pointer to the command structure to modify.
 * @return uint8_t Returns 1 if successful, 0 if the command pointer is null.
 */
uint8_t robokit_drive_command_enable_imu(S_command *cmd);

/**
 * @brief Disables the IMU flag for the given command.
 *
 * This function modifies the provided command to disable the IMU flag.
 * It accesses the flags field of the command and clears the IMU bit,
 * effectively disabling the IMU functionality for this command.
 *
 * @param[out] cmd S_command* A pointer to the command structure that will be modified.
 * @return uint8_t Returns 1 if the operation is successful, and 0 if the cmd is null.
 */
uint8_t robokit_drive_command_disable_imu(S_command *cmd);

#endif //DRIVE_COMMAND_H
