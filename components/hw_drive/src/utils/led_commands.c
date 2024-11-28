//
// Created by Thomas Abplanalp on 15.11.24.
//

#include "led_command.h"
#include "command_data_containers.h"
#include "robokit_log.h"
#include <led_strip.h>
#include <scheduler.h>


#ifndef LED_PIN_ST
#define LED_PIN_ST 13
#endif


extern void _robokit_command_init(S_command *cmd);

static led_strip_handle_t _my_test_led_strip;

void _robokit_setup_led_callback(_S_Command_LED *cmd, uint8_t mode, uint8_t *flags) {
	*flags = 0;
	if(mode == E_SCHEDULE_MODE_PERFORM) {
		for(int e=0;e<LED_MAX_COUNT;e++) {
			if(cmd->LEDs & (1 << e)) {
				led_strip_set_pixel(_my_test_led_strip, e, cmd->red, cmd->green, cmd->blue);
			}
		}
	}
}

void _robokit_flush_led_callback(S_command *cmd, uint8_t mode, uint8_t *flags) {
	*flags = 0;
	if(mode == E_SCHEDULE_MODE_PERFORM) {
		if(cmd->data[0]) {
			led_strip_clear(_my_test_led_strip);
		} else {
			led_strip_refresh(_my_test_led_strip);
		}
	}
}

void _led_commands_init(void) {
	robokit_register_command_fn(E_COMMAND_LED_SETUP, _robokit_setup_led_callback);
	robokit_register_command_fn(E_COMMAND_LED_FLUSH, _robokit_flush_led_callback);

	led_strip_config_t strip_config = {
		 .strip_gpio_num = LED_PIN_ST,
		.max_leds = LED_MAX_COUNT,
	};

	led_strip_rmt_config_t rmt_config = {
		.resolution_hz = 10 * 1000 * 1000, // 10MHz
		.flags.with_dma = false,
	};

	ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &_my_test_led_strip));
}


uint8_t robokit_make_led_command_setup(S_command *cmd, uint8_t LED_number, uint8_t red, uint8_t green, uint8_t blue) {
 	uint32_t leds = 0;
	ROBOKIT_MAKE_LED_LIST(leds, LED_number);

	return robokit_make_led_command_setup_list(cmd, leds, red, green, blue);
}

uint8_t robokit_make_led_command_setup_list(S_command *cmd, uint32_t LED_numbers, uint8_t red, uint8_t green, uint8_t blue) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	cmd->cmd = E_COMMAND_LED_SETUP;
	_S_Command_LED *led_cmd = (_S_Command_LED *)cmd;
	led_cmd->LEDs = LED_numbers;
	led_cmd->red = red;
	led_cmd->green = green;
	led_cmd->blue = blue;

	return 1;
}

uint8_t robokit_make_led_command_flush(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	cmd->cmd = E_COMMAND_LED_FLUSH;
	cmd->data[0] = 0;

	return 1;
}

uint8_t robokit_make_led_command_clear(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	cmd->cmd = E_COMMAND_LED_FLUSH;
	cmd->data[0] = 1;

	return 1;
}