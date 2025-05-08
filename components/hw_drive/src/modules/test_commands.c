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

#include <private/command_data_containers.h>
#include <stdio.h>
#include "test_commands.h"
#include <hal/scheduler.h>

#include <private/robokit_log.h>
#include <led_strip.h>
#include <modules/robokit_module.h>

#ifndef LED_PIN
#define LED_PIN 8
#endif

#if !ROBOKIT_USE_BUILTIN_LED
#include "driver/gpio.h"
#endif

ROBOKIT_CHECK_COMMAND_STRUCT(_S_Command_test);

static volatile uint8_t _test_led_current_state = 0;
static led_strip_handle_t _my_test_led_strip;

/**
 * Callback function that handles LED control based on the provided command and mode.
 *
 * This function is invoked as part of a command processing routine. It interprets the given
 * command and mode to control an LED by either turning it on or off. If the system is
 * configured to use a built-in LED strip, the function sets the LED strip's pixel color
 * and refreshes it. Otherwise, it controls the GPIO pin directly associated with the LED.
 * The result of the operation is logged at the INFO or WARNING level.
 *
 * @param cmd Pointer to the command structure containing the specific command to be executed.
 * @param mode Operation mode indicating whether the command is in pre-check or perform mode.
 *             This function reacts only when the mode is E_SCHEDULE_MODE_PERFORM.
 * @param flags Pointer to a flags variable which gets reset (set to zero) at the start of the function,
 *              used for returning status flags if necessary.
 */
ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_TEST, _S_Command_test *cmd, uint8_t mode, uint8_t *flags) {
	*flags = 0;

	if(mode == E_SCHEDULE_MODE_PERFORM) {
#if ROBOKIT_USE_BUILTIN_LED
		if(ROBOKIT_CMD_CAST(_S_Command_test*, cmd)->reserved1) {
			ROBOKIT_LOGI("Test LED ON");
			led_strip_set_pixel(_my_test_led_strip, 0, 16, 16, 16);
			led_strip_refresh(_my_test_led_strip);
		} else {
			ROBOKIT_LOGI("Test LED OFF");
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


/**
 * Initializes the test commands for the Robokit platform.
 *
 * This function configures the LED control settings
 * depending on whether a built-in LED is being used. If `ROBOKIT_USE_BUILTIN_LED` is defined, it
 * initializes the LED strip configuration for a DevkitC board, specifying pin, maximum number
 * of LEDs, and resolution. If the built-in LED is not used, it sets the direction of the LED pin
 * as an output for direct GPIO control.
 *
 * This setup is necessary for handling test commands related to LED control operations.
 */
ROBOKIT_MODULE_INIT() {
#if ROBOKIT_USE_BUILTIN_LED
	// DevkitC Board configuration
	led_strip_config_t strip_config = {
		.strip_gpio_num = LED_PIN,
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

/**
 * @inheritDoc
 */
uint8_t robokit_make_test_command(S_command *cmd) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);

	cmd->cmd = E_COMMAND_TEST;

	if(_test_led_current_state) {
		_test_led_current_state = 0;
		ROBOKIT_CMD_CAST(_S_Command_test *, cmd)->reserved1 = 0;
	} else {
		_test_led_current_state = 1;
		ROBOKIT_CMD_CAST(_S_Command_test *, cmd)->reserved1 = 1;
	}
	return 1;
}