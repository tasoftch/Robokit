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

#include <private/motor_logic.h>

#include "private/p_lut.h"
#include "private/n_lut.h"

/**
 * @inheritDoc
 */
uint8_t robokit_vector_to_motor(S_vector vector, S_motor_ctl *pwm_motor_left, S_motor_ctl *pwm_motor_right) {
    int8_t l, r;
    
    // Die Länge des Vektors darf ROBOKIT_MAX_SPEED nicht überschreiten.
    if(vector.speed > ROBOKIT_MAX_SPEED || vector.speed < ROBOKIT_MIN_SPEED)
        return 0;
    
    if(vector.angle >= 0 && vector.angle > 60)
        return 0;
    if(vector.angle < 0 && vector.angle < -60)
        return 0;
    
    if(vector.angle >= 0) {
        l = _motor_lut_pwm_positive_deg[vector.angle][0];
        r = _motor_lut_pwm_positive_deg[vector.angle][1];
    } else {
        l = _motor_lut_pwm_negative_deg[-vector.angle][0];
        r = _motor_lut_pwm_negative_deg[-vector.angle][1];
    }
    
    if(pwm_motor_left) {
        if(l < 0) {
            pwm_motor_left->direction = 0;
            pwm_motor_left->speed = -l * vector.speed / 100;
        } else {
            pwm_motor_left->direction = 1;
            pwm_motor_left->speed = l * vector.speed / 100;
        }
    }
    
    if(pwm_motor_right) {
        if(r < 0) {
            pwm_motor_right->direction = 0;
            pwm_motor_right->speed = -r * vector.speed / 100;
        } else {
            pwm_motor_right->direction = 1;
            pwm_motor_right->speed = r * vector.speed / 100;
        }
    }
    
    return 1;
}
