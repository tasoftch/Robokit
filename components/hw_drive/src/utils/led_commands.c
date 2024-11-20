//
// Created by Thomas Abplanalp on 15.11.24.
//

#include "led_command.h"
#include "command_data_containers.h"
#include "robokit_log.h"

extern void _robokit_command_init(S_command *cmd);


void _led_commands_init(void) {
	ROBOKIT_LOGI("led_commands_init");
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

	return 1;
}