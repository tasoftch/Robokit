//
// Created by Thomas Abplanalp on 18.11.24.
//

#ifndef LED_PIN
#define LED_PIN 8
#endif
#include <command_data_containers.h>
#include <stdio.h>
#include "test_commands.h"
#include "scheduler.h"
#include "driver/gpio.h"

extern void _robokit_command_init(S_command *cmd);

static volatile uint8_t _test_led_current_state = 0;


void __robokit_my_callback(S_command *cmd, uint8_t mode, uint8_t *flags) {
	*flags = 0;
	if(mode == E_SCHEDULE_MODE_PERFORM) {
		if(_ROBOKIT_CMD_CAST(_S_Command_test*, cmd)->reserved1) {
			gpio_set_level(LED_PIN, 1);
		} else {
			gpio_set_level(LED_PIN, 0);
		}
	}
}


void _test_commands_init(void) {
	printf("Test Size %d with LED Pin %d\n", sizeof(_S_Command_test), LED_PIN);

	robokit_register_command_fn(E_COMMAND_TEST, __robokit_my_callback);

	gpio_set_direction(LED_PIN, GPIO_MODE_OUTPUT);
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