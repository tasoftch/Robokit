//
//  motor_logic.c
//  robokit
//
//  Created by Thomas Abplanalp on 10.11.24.
//

#include "motor_logic.h"

#include "p_lut.h"
#include "n_lut.h"

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
