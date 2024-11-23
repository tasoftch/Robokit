//
// Created by Thomas Abplanalp on 18.11.24.
//

#include <command_data_containers.h>
#include <stdio.h>
#include "test_commands.h"
#include "scheduler.h"

#include "robokit_log.h"
#include <led_strip.h>

#ifndef LED_PIN
#define LED_PIN 8
#endif

#if !ROBOKIT_USE_BUILTIN_LED
#include "driver/gpio.h"
#endif

extern void _robokit_command_init(S_command *cmd);

static volatile uint8_t _test_led_current_state = 0;
static led_strip_handle_t _my_test_led_strip;

void __robokit_my_callback(S_command *cmd, uint8_t mode, uint8_t *flags) {
	*flags = 0;

	if(mode == E_SCHEDULE_MODE_PERFORM) {
#if ROBOKIT_USE_BUILTIN_LED
		if(_ROBOKIT_CMD_CAST(_S_Command_test*, cmd)->reserved1) {
			ROBOKIT_LOGW("Test LED ON");
			led_strip_set_pixel(_my_test_led_strip, 0, 16, 16, 16);
			led_strip_refresh(_my_test_led_strip);
		} else {
			ROBOKIT_LOGW("Test LED OFF");
			led_strip_clear(_my_test_led_strip);
		}
#else
		if(_ROBOKIT_CMD_CAST(_S_Command_test*, cmd)->reserved1) {
			gpio_set_level(LED_PIN, 1);
			ROBOKIT_LOGW("Test LED (Pin %d) ON", LED_PIN);
		} else {
			gpio_set_level(LED_PIN, 0);
			ROBOKIT_LOGW("Test LED (Pin %d) OFF", LED_PIN);
		}
#endif
	}
}


void _test_commands_init(void) {
	ROBOKIT_LOGI("_test_commands_init");
	printf("Test Size %d with LED Pin %d\n", sizeof(_S_Command_test), LED_PIN);
	robokit_register_command_fn(E_COMMAND_TEST, __robokit_my_callback);


#if ROBOKIT_USE_BUILTIN_LED
	// DevkitC Board configuration
	led_strip_config_t strip_config = {
		.strip_gpio_num = 8,
		.max_leds = 1,
	};

	led_strip_rmt_config_t rmt_config = {
		.resolution_hz = 10 * 1000 * 1000, // 10MHz
		.flags.with_dma = false,
	};
	ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &_my_test_led_strip));
#else
	gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
#endif
}


uint8_t robokit_make_test_command(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	cmd->cmd = E_COMMAND_TEST;

	if(_test_led_current_state) {
		_test_led_current_state = 0;
		_ROBOKIT_CMD_CAST(_S_Command_test *, cmd)->reserved1 = 0;
	} else {
		_test_led_current_state = 1;
		_ROBOKIT_CMD_CAST(_S_Command_test *, cmd)->reserved1 = 1;
	}
	return 1;
}