//
// Created by Thomas Abplanalp on 15.04.25.
//

#include "hal/hc_sr04.h"

#include <rom/ets_sys.h>

#include "driver/gpio.h"
#include "esp_timer.h"

#define TRIGGER_GPIO 19

static const char *TAG = "HC-SR04";

void hc_sr04_init() {
	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << TRIGGER_GPIO),
		.mode = GPIO_MODE_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_DISABLE
	};
	gpio_config(&io_conf);
}

float hc_sr04_read_distance_cm() {
	// Send 10µs pulse to trigger
	gpio_set_direction(TRIGGER_GPIO, GPIO_MODE_OUTPUT);
	gpio_set_level(TRIGGER_GPIO, 0);
	ets_delay_us(2);
	gpio_set_level(TRIGGER_GPIO, 1);
	ets_delay_us(10);
	gpio_set_level(TRIGGER_GPIO, 0);

	gpio_set_direction(TRIGGER_GPIO, GPIO_MODE_INPUT);

	int64_t start_time = 0;
	int64_t end_time = 0;
	int timeout = 30000;  // 30ms timeout

	// Wait for pin to go HIGH (start of echo)
	while (gpio_get_level(TRIGGER_GPIO) == 0 && timeout-- > 0)
	{
		ets_delay_us(1);
	}
	if (timeout <= 0) return -1;

	start_time = esp_timer_get_time();

	timeout = 30000;
	while (gpio_get_level(TRIGGER_GPIO) == 1 && timeout-- > 0)
	{
		ets_delay_us(1);
	}
	if (timeout <= 0) return -2;

	end_time = esp_timer_get_time();
	float pulse_duration = (float)(end_time - start_time); // in µs
	return pulse_duration / 58.0f; // ~58 µs per cm
}