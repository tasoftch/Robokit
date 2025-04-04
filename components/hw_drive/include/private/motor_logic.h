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

#ifndef motor_logic_h
#define motor_logic_h

#include "config.h"
#include "vector.h"

// Structures S_motor_ctl define how fast and in which direction (forward or backward)
// a target motor must be controlled.
typedef struct {
    uint8_t direction:1;            // 1: forward, 0: backward
    uint8_t speed:7;                // 0 Hold, 100 As fast as possible
} S_motor_ctl;

/**
 * @brief Converts a vector into motor speed and direction controls for two motors.
 *
 * This function translates the input vector, consisting of angle and speed,
 * into control signals for two motors, controlling their speed and direction.
 *
 * @param vector The input vector with angle and speed.
 * @param pwm_motor_left A Pointer to the motor control structure for the left motor.
 * @param pwm_motor_right A Pointer to the motor control structure for the right motor.
 * @return Returns 1 if the conversion is successful, otherwise returns 0.
 */
uint8_t robokit_vector_to_motor(S_vector vector, S_motor_ctl *pwm_motor_left, S_motor_ctl *pwm_motor_right);

#endif /* motor_logic_h */
