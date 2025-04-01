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

#ifndef IMU_COMMANDS_H
#define IMU_COMMANDS_H

#include "main_commands.h"

// The IMU must be enabled for obtaining information in values.h
uint8_t robokit_make_command_imu_enable(S_command *command);
uint8_t robokit_make_command_imu_disable(S_command *command);

// The calibration is one cycle only to reset the current orientation to 0.
// Turning the device to the left reduces the angle until 1800° (10 rounds).
// Turning the device to the right increases the angle until 1800° (10 rounds).

uint8_t robokit_make_command_imu_calibration(S_command *command, void (*done_callback)(void));

// Giving a drive command enables the IMU and applies a calibration.
// The system will now follow 0° and inject commands to correct the angle.
uint8_t robokit_make_command_imu_drive_forward(S_command *command, T_Speed speed);
uint8_t robokit_make_command_imu_drive_backward(S_command *command, T_Speed speed);

uint8_t robokit_make_command_imu_stop(S_command *command);

#endif //IMU_COMMANDS_H
