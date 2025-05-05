//
// Created by Thomas Abplanalp on 15.04.25.
//

#include "hc_sr04.h"

#include <rom/ets_sys.h>

#include "driver/gpio.h"
#include "esp_timer.h"

#define TRIGGER_GPIO GPIO_NUM_9
#define ECHO_GPIO    GPIO_NUM_8

static const char *TAG = "HC-SR04";

void hc_sr04_init() {
	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << TRIGGER_GPIO),
		.mode = GPIO_MODE_OUTPUT,
	};
	gpio_config(&io_conf);

	io_conf.pin_bit_mask = (1ULL << ECHO_GPIO);
	io_conf.mode = GPIO_MODE_INPUT;
	gpio_config(&io_conf);
}

float hc_sr04_read_distance_cm() {
	// Send 10µs pulse to trigger
	gpio_set_level(TRIGGER_GPIO, 0);
	ets_delay_us(2);
	gpio_set_level(TRIGGER_GPIO, 1);
	ets_delay_us(10);
	gpio_set_level(TRIGGER_GPIO, 0);

	// Wait for echo to go HIGH
	int64_t start_time = esp_timer_get_time();
	while (gpio_get_level(ECHO_GPIO) == 0) {
		if ((esp_timer_get_time() - start_time) > 100000) return -1;
	}

	int64_t echo_start = esp_timer_get_time();
	while (gpio_get_level(ECHO_GPIO) == 1) {
		if ((esp_timer_get_time() - echo_start) > 30000) return -1;
	}
	int64_t echo_end = esp_timer_get_time();

	float pulse_duration = (float)(echo_end - echo_start); // in µs
	float distance_cm = pulse_duration / 58.0f; // ~58 µs per cm
	return distance_cm;
}