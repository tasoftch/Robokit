//
//  motor_logic.h
//  robokit
//
//  Created by Thomas Abplanalp on 10.11.24.
//

#ifndef motor_logic_h
#define motor_logic_h

#include <stdio.h>

#include "types.h"
#include "vector.h"

// Strukturen dieser Art liegen der Motorsteuerung an.
// Sie definiert die Richtung und Geschwindigkeit des Motors.
typedef struct {
    uint8_t direction:1;            // 1: vorwärts, 0: rückwärts
    uint8_t speed:7;                // 0 Stillstand, 100 Vollgas
} S_motor_ctl;

// Motor-Konfigurationen berechnen lassen aus Vektoren.
uint8_t robokit_vector_to_motor(S_vector vector, S_motor_ctl *pwm_motor_left, S_motor_ctl *pwm_motor_right);

#endif /* motor_logic_h */
