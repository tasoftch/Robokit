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

#include "led_command.h"
#include <private/command_data_containers.h>
#include <led_strip.h>
#include <hal/scheduler.h>
#include <modules/robokit_module.h>


#ifndef LED_PIN_ST
#define LED_PIN_ST 16
#endif

static led_strip_handle_t _my_test_led_strip;

/**
 * This function sets up the LED strip based on the command parameters.
 * It configures each LED's color in the strip according to the specified settings.
 *
 * @param cmd A pointer to the S_command structure containing the LED setup command,
 *            including the color (red, green, blue) and the LEDs that should be set.
 * @param mode The mode in which the function operates. The function performs LED setup
 *             actions when the mode is E_SCHEDULE_MODE_PERFORM.
 * @param flags A pointer to a uint8_t to store additional status or result flags
 *              after executing the LED setup. The function initializes the flags to 0.
 */
ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_LED_SETUP, _S_Command_LED *cmd, uint8_t mode, uint8_t *flags) {
	*flags = 0;
	return;
	if(mode == E_SCHEDULE_MODE_PERFORM) {
		for(int e=0;e<LED_MAX_COUNT;e++) {
			if(cmd->LEDs & (1 << e)) {
				led_strip_set_pixel(_my_test_led_strip, e, cmd->red, cmd->green, cmd->blue);
			}
		}
	}
}

/**
 * This function flushes the LED strip based on the command parameters.
 * Depending on the input command data, it either clears or refreshes the LED strip.
 *
 * @param cmd A pointer to the S_command structure containing the command data,
 *            where cmd->data[0] determines the action: clear if true, refresh if false.
 * @param mode The mode in which the function operates. The flushing actions
 *             are performed when the mode is E_SCHEDULE_MODE_PERFORM.
 * @param flags A pointer to a uint8_t where additional status or result flags
 *              may be stored. The function sets these flags to 0.
 */
ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_LED_FLUSH, S_command *cmd, uint8_t mode, uint8_t *flags) {
	*flags = 0;
	return;
	if(mode == E_SCHEDULE_MODE_PERFORM) {
		if(cmd->data[0]) {
			led_strip_clear(_my_test_led_strip);
		} else {
			led_strip_refresh(_my_test_led_strip);
		}
	}
}

/**
 * Initializes the LED command functionality by configuring the LED strip
 * settings.
 *
 * It sets the GPIO number and maximum number of LEDs for the LED strip, then creates
 * a new remoter device for the LED strip with a specified RMT configuration.
 *
 * The RMT configuration includes parameters such as resolution and DMA usage.
 * The setup ensures that the LED strip is correctly initialized and ready to handle
 * incoming LED commands.
 */
ROBOKIT_MODULE_INIT() {
	return;
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

/**
 * @inheritDoc
 */
uint8_t robokit_make_led_command_setup(S_command *cmd, uint8_t LED_number, uint8_t red, uint8_t green, uint8_t blue) {
 	uint32_t leds = 0;
	ROBOKIT_MAKE_LED_LIST(leds, LED_number);

	return robokit_make_led_command_setup_list(cmd, leds, red, green, blue);
}

/**
 * @inheritDoc
 */
uint8_t robokit_make_led_command_setup_list(S_command *cmd, uint32_t LED_numbers, uint8_t red, uint8_t green, uint8_t blue) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);

	cmd->cmd = E_COMMAND_LED_SETUP;
	_S_Command_LED *led_cmd = ROBOKIT_CMD_CAST(_S_Command_LED*, cmd);
	led_cmd->LEDs = LED_numbers;
	led_cmd->red = red;
	led_cmd->green = green;
	led_cmd->blue = blue;

	return 1;
}

/**
 * @inheritDoc
 */
uint8_t robokit_make_led_command_flush(S_command *cmd) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);

	cmd->cmd = E_COMMAND_LED_FLUSH;
	cmd->data[0] = 0;

	return 1;
}

/**
 * @inheritDoc
 */
uint8_t robokit_make_led_command_clear(S_command *cmd) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);

	cmd->cmd = E_COMMAND_LED_FLUSH;
	cmd->data[0] = 1;

	return 1;
}