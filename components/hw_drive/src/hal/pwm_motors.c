//
// Created by Thomas Abplanalp on 18.11.24.
//

#include "pwm_motors.h"

#include <robokit_log.h>
#include <stdio.h>
#include <driver/ledc.h>


// Zu steuernde Pins
// {M0+, M0-, M1+, M1-, ...}
#if ROBOKIT_USE_MOTOR_1
int pwmPins[] = {16, 23, 22, 21, 20, 19, 18, 17};
#else
int pwmPins[] = {22, 21, 20, 19, 18, 17};
#endif
// 16 geht nicht
// 17 Geht


#define NUM_PINS (sizeof(pwmPins) / sizeof(pwmPins[0]))

void _robokit_setup_pin(uint8_t i) {

	ledc_channel_config_t ledc_channel = {
		.channel    = i,               // LEDC-Kanalnummer
		.duty       = PWM_DUTY_CYCLE,  // Anfangs-Duty-Cycle
		.gpio_num   = pwmPins[i],      // Zu steuernder Pin
		.speed_mode = LEDC_MODE,       // Modus
		.hpoint     = 0,               // Standardwert
		.timer_sel  = LEDC_TIMER       // Timer
	};

	if(ledc_channel_config(&ledc_channel) != ESP_OK) {
		ROBOKIT_LOGE("SETUP %u %d failed",i,  pwmPins[i]);
	}
}

void _robokit_pwm_motors_init(void) {
	ROBOKIT_LOGI("robokit_pwm_motors_init");


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

static uint8_t _robokit_assign_pins(E_motor motor, uint8_t *motor_plus, uint8_t *motor_minus) {
	if(motor < NUM_PINS / 2) {
		uint8_t mt_idx = motor * 2;
		*motor_plus = mt_idx;
		*motor_minus = mt_idx+1;
		return 1;
	}
	return 0;
}

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

void robokit_pwm_motor_all_off(void) {
#if ROBOKIT_USE_MOTOR_1
	robokit_pwm_motor_off(E_DRIVE_MOTOR_1);
#endif
	robokit_pwm_motor_off(E_DRIVE_MOTOR_2);
	robokit_pwm_motor_off(E_DRIVE_MOTOR_3);
	robokit_pwm_motor_off(E_DRIVE_MOTOR_4);
}

void robokit_pwm_motor_on(E_motor motor, uint8_t flags) {
	robokit_pwm_motor_speed(motor, 255, flags);
}

void robokit_pwm_motor_off(E_motor motor) {
	uint8_t mp=0, mm=0;
	if(_robokit_assign_pins(motor, &mp, &mm)) {
		_robokit_send_pwm(mp, 0);
		_robokit_send_pwm(mm, 0);
	}
}

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
			duty = 255 - duty;
			nduty = 255;
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

void robokit_pwm_motor_1_speed(T_Speed speed, uint8_t flags) {
#if ROBOKIT_USE_MOTOR_1
	robokit_pwm_motor_speed(E_DRIVE_MOTOR_1, speed, flags);
#endif
}

void robokit_pwm_motor_2_speed(T_Speed speed, uint8_t flags) {
	robokit_pwm_motor_speed(E_DRIVE_MOTOR_2, speed, flags);
}
void robokit_pwm_motor_3_speed(T_Speed speed, uint8_t flags) {
	robokit_pwm_motor_speed(E_DRIVE_MOTOR_3, speed, flags);
}
void robokit_pwm_motor_4_speed(T_Speed speed, uint8_t flags) {
	robokit_pwm_motor_speed(E_DRIVE_MOTOR_4, speed, flags);
}