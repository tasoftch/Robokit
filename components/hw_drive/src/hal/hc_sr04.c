//
// Created by Thomas Abplanalp on 15.04.25.
//

#include "hal/hc_sr04.h"

#include <esp_attr.h>
#include <portmacro.h>
#include <rom/ets_sys.h>

#include "driver/gpio.h"
#include "esp_timer.h"

#define TRIGGER_GPIO 19

static const char *TAG = "HC-SR04";
static portMUX_TYPE lock_10_us = portMUX_INITIALIZER_UNLOCKED;

volatile int64_t echo_start = 0;
volatile int64_t echo_end = 0;
volatile bool echo_done = false;
volatile bool run_isr = false;

static void IRAM_ATTR echo_isr_handler(void* arg)
{
	if(run_isr) {
		int level = gpio_get_level(TRIGGER_GPIO);
		int64_t now = esp_timer_get_time();

		if (level == 1) {
			echo_start = now;
		} else {
			echo_end = now;
			echo_done = true;
		}
	}
}

void hc_sr04_init() {
	gpio_config_t io_conf = {
		.pin_bit_mask = (1ULL << TRIGGER_GPIO),
		.mode = GPIO_MODE_INPUT_OUTPUT,
		.pull_up_en = GPIO_PULLUP_DISABLE,
		.pull_down_en = GPIO_PULLDOWN_DISABLE,
		.intr_type = GPIO_INTR_ANYEDGE
	};
	gpio_config(&io_conf);

	gpio_install_isr_service(0);  // Default config
	gpio_isr_handler_add(TRIGGER_GPIO, echo_isr_handler, NULL);
}

static void hc_sr04_trigger_sensor(void) {
	gpio_set_direction(TRIGGER_GPIO, GPIO_MODE_OUTPUT);
	gpio_set_level(TRIGGER_GPIO, 0);
	ets_delay_us(2);

	portENTER_CRITICAL(&lock_10_us);
	gpio_set_level(TRIGGER_GPIO, 1);
	ets_delay_us(10);
	gpio_set_level(TRIGGER_GPIO, 0);
	portEXIT_CRITICAL(&lock_10_us);

	gpio_set_direction(TRIGGER_GPIO, GPIO_MODE_INPUT);
}

static esp_err_t hc_sr04_await_response(void) {
	echo_done = false;
	run_isr = true;
	int timeout = 30000;
	while (!echo_done && --timeout > 0) {
		ets_delay_us(1);
	}
	run_isr = false;
	return timeout > 0 ? ESP_OK : ESP_ERR_TIMEOUT;
}

uint16_t hc_sr04_read_distance_cm() {
	hc_sr04_trigger_sensor();

	if (hc_sr04_await_response() != ESP_OK)
		return -1;

	int64_t pulse_duration = (echo_end - echo_start); // in µs
	return pulse_duration / 58; // ~58 µs per cm
}