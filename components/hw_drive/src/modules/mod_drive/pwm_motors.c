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

#include "hal/pwm_motors.h"

#include <private/robokit_log.h>
#include <stdio.h>
#include <driver/ledc.h>
#include <modules/robokit_module.h>

/*
 *		20: MOTOR 1 -
 *		21: MOTOR 1 +
 *		22: MOTOR 0 -
 *		23: MOTOR 0 +
 */
static int _pwmPins[] = {20, 21, 22, 23};


#define NUM_PINS (sizeof(_pwmPins) / sizeof(_pwmPins[0]))

void _robokit_setup_pin(uint8_t i) {

	ledc_channel_config_t ledc_channel = {
		.channel    = i,               // LEDC-Kanalnummer
		.duty       = PWM_DUTY_CYCLE,  // Anfangs-Duty-Cycle
		.gpio_num   = _pwmPins[i],      // Zu steuernder Pin
		.speed_mode = LEDC_MODE,       // Modus
		.hpoint     = 0,               // Standardwert
		.timer_sel  = LEDC_TIMER       // Timer
	};

	if(ledc_channel_config(&ledc_channel) != ESP_OK) {
		ROBOKIT_LOGE("SETUP %u %d failed",i,  _pwmPins[i]);
	}
}

/**
 * Registered function to start up the drive motors and pwm controls.
 */
ROBOKIT_MODULE_INIT() {
	printf("robokit_pwm_motors_init\n");

	ledc_timer_config_t ledc_timer = {
		.duty_resolution = PWM_RESOLUTION, // Auflösung der PWM-Duty
		.freq_hz = PWM_FREQ,               // PWM-Frequenz
		.speed_mode = LEDC_MODE,           // PWM-Modus (High oder Low Speed)
		.timer_num = LEDC_TIMER            // Timer
	};


	esp_err_t err = ledc_timer_config(&ledc_timer);
	if(err != ESP_OK) {
		ROBOKIT_LOGE("Could not configure ledc timers");
		return;
	}

	for (int i = 0; i < NUM_PINS; i++) {
		_robokit_setup_pin(i);
	}

	robokit_pwm_motor_all_off();
}

/**
 * @brief Assign motor to configured wires
 *
 * @param motor Motor number
 * @param motor_plus Assigned wire for plus terminal
 * @param motor_minus Assigned wire for minus terminal
 * @return 1 on success, 0 otherwise
 */
static uint8_t _robokit_assign_pins(E_motor motor, uint8_t *motor_plus, uint8_t *motor_minus) {
	if(motor < NUM_PINS / 2) {
		uint8_t mt_idx = motor * 2;
		*motor_plus = mt_idx;
		*motor_minus = mt_idx+1;
		return 1;
	}
	return 0;
}

/**
 * @brief Changes configuration to drive a specified GPIO with a specific duty cycle.
 *
 * @param pin Wired GPIO pin to motor driver
 * @param duty The duty cycle to drive at the specified pin
 */
static void _robokit_send_pwm(uint8_t pin, uint8_t duty) {
	esp_err_t err = ledc_set_duty(LEDC_MODE, pin, duty);
	if(err == ESP_OK) {
		err = ledc_update_duty(LEDC_MODE, pin);
		if(err == ESP_OK) {
			// ROBOKIT_LOGI("PWM set %d on channel %d pin %d", duty, pin, pwmPins[pin]);
			return;
		}
	}

	ROBOKIT_LOGE("PWM error: %d for pin %d and duty %d", err, pin, duty);
}

/**
 * @inheritDoc
 */
void robokit_pwm_motor_all_off(void) {
	robokit_pwm_motor_off(E_DRIVE_MOTOR_1);
	robokit_pwm_motor_off(E_DRIVE_MOTOR_2);
}

/**
 * @inheritDoc
 */
void robokit_pwm_motor_on(E_motor motor, uint8_t flags) {
	robokit_pwm_motor_speed(motor, 255, flags);
}

/**
 * @inheritDoc
 */
void robokit_pwm_motor_off(E_motor motor) {
	uint8_t mp=0, mm=0;
	if(_robokit_assign_pins(motor, &mp, &mm)) {
		_robokit_send_pwm(mp, 0);
		_robokit_send_pwm(mm, 0);
	}
}

/**
 * @inheritDoc
 */
void robokit_pwm_motor_speed(E_motor motor, T_Speed speed, uint8_t flags) {
	uint8_t mp=0, mm=0;
	if(_robokit_assign_pins(motor, &mp, &mm)) {
		uint8_t duty = 0;
		uint8_t nduty = 0;

		if(flags & E_DRIVE_MOTOR_DR_LOW_FLAG) {
			duty = speed * 255 / 100;
			nduty = 0;
		}

		if(flags & E_DRIVE_MOTOR_DR_HIGH_FLAG) {
			nduty = 255 - duty;
			duty = 255;
		}

		if(flags & E_DRIVE_MOTOR_FORWARD_FLAG) {
			if(flags & E_DRIVE_MOTOR_SWITCH_CONNECTORS) {
				_robokit_send_pwm(mm, duty);
				_robokit_send_pwm(mp, nduty);
			} else {
				_robokit_send_pwm(mp, duty);
				_robokit_send_pwm(mm, nduty);
			}
		}

		if(flags & E_DRIVE_MOTOR_BACKWARD_FLAG) {
			if(flags & E_DRIVE_MOTOR_SWITCH_CONNECTORS) {
				_robokit_send_pwm(mm, nduty);
				_robokit_send_pwm(mp, duty);
			} else {
				_robokit_send_pwm(mp, nduty);
				_robokit_send_pwm(mm, duty);
			}
		}
	}
}

/**
 * @inheritDoc
 */
void robokit_pwm_motor_1_speed(T_Speed speed, uint8_t flags) {
	robokit_pwm_motor_speed(E_DRIVE_MOTOR_1, speed, flags);
}

/**
 * @inheritDoc
 */
void robokit_pwm_motor_2_speed(T_Speed speed, uint8_t flags) {
	robokit_pwm_motor_speed(E_DRIVE_MOTOR_2, speed, flags);
}