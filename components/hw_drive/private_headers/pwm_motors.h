//
// Created by Thomas Abplanalp on 18.11.24.
//

#ifndef PWM_MOTORS_H
#define PWM_MOTORS_H

#include <config.h>

#define PWM_FREQ       20000    // PWM-Frequenz in Hz
#define PWM_RESOLUTION LEDC_TIMER_8_BIT // Auflösung: 8 Bit
#define PWM_DUTY_CYCLE 64

#define LEDC_TIMER      LEDC_TIMER_1
#define LEDC_MODE       LEDC_LOW_SPEED_MODE

// Motor 1 covers USB port.
// If used, no debug information will be sent to console.
// idf.py monitor will not work anymore.
#define ROBOKIT_USE_MOTOR_1 0

typedef enum {
#if ROBOKIT_USE_MOTOR_1
	E_DRIVE_MOTOR_1,
#endif
	E_DRIVE_MOTOR_2,
	E_DRIVE_MOTOR_3,
	E_DRIVE_MOTOR_4
} E_motor;

enum {
	E_DRIVE_MOTOR_FORWARD_FLAG			= 1<<0,
	E_DRIVE_MOTOR_BACKWARD_FLAG			= 1<<1,
	E_DRIVE_MOTOR_DR_HIGH_FLAG			= 1<<2,
	E_DRIVE_MOTOR_DR_LOW_FLAG			= 1<<3,
	E_DRIVE_MOTOR_SWITCH_CONNECTORS		= 1<<4,
};

void robokit_pwm_motor_all_off(void);

void robokit_pwm_motor_on(E_motor motor, uint8_t flags);
void robokit_pwm_motor_off(E_motor motor);

void robokit_pwm_motor_speed(E_motor motor, T_Speed speed, uint8_t flags);

void robokit_pwm_motor_1_speed(T_Speed speed, uint8_t flags);
void robokit_pwm_motor_2_speed(T_Speed speed, uint8_t flags);
void robokit_pwm_motor_3_speed(T_Speed speed, uint8_t flags);
void robokit_pwm_motor_4_speed(T_Speed speed, uint8_t flags);

#endif //PWM_MOTORS_H
