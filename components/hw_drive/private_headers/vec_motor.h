//
//  vec_motor.h
//  robokit
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#ifndef vec_motor_h
#define vec_motor_h

#include "types.h"
#include "vector.h"
#include "motor_logic.h"


#define ROBOKIT_MOTOR_1 0x00
#define ROBOKIT_MOTOR_2 0x01
#define ROBOKIT_MOTOR_3 0x02
#define ROBOKIT_MOTOR_4 0x03


// Motor-Konfigurationen laden
// Achtung!
// Diese Funktion schreibt die Konfiguration sofort ins Motor-Register.
// Sie darf nur über die Kommando Kette aufgerufen werden!
uint8_t     robokit_motor_set_config(uint8_t motor, S_motor_ctl control);

// Liest die aktuelle Konfiguration für einen Motor aus.
S_motor_ctl robokit_motor_get_config(uint8_t motor);

#endif /* vec_motor_h */
