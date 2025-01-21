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
 * The motor configuration is required to assign motor numbers to motor left and motor right.
 */
typedef struct {
    uint8_t motor_1:1;
    uint8_t motor_2:1;
    uint8_t motor_3:1;
    uint8_t motor_4:1;
    uint8_t fast_decay:1;
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
