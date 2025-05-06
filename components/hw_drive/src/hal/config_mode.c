//
// Created by Thomas Abplanalp on 06.05.25.
//
#include "device.h"
#include "driver/gpio.h"
#include "modules/robokit_module.h"

#define ROBOKIT_CONFIG_PIN GPIO_NUM_15

ROBOKIT_MODULE_INIT() {
	gpio_config_t io_conf;
	io_conf.intr_type = GPIO_INTR_DISABLE;
	io_conf.mode = GPIO_MODE_INPUT;
	io_conf.pin_bit_mask = (1ULL << ROBOKIT_CONFIG_PIN);
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
	gpio_config(&io_conf);
}


uint8_t robokit_system_in_config_mode(void) {
	return gpio_get_level(ROBOKIT_CONFIG_PIN) ? 0 : 1;
}