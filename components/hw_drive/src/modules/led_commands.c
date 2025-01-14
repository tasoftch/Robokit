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
#include <private/robokit_log.h>
#include <led_strip.h>
#include <hal/scheduler.h>


#ifndef LED_PIN_ST
#define LED_PIN_ST 13
#endif


extern void _robokit_command_init(S_command *cmd);

static led_strip_handle_t _my_test_led_strip;

/**
 * This function sets up the LED strip based on the command parameters.
 * It configures each LED's color in the strip according to the specified settings.
 *
 * @param cmd A pointer to the _S_Command_LED structure containing the LED setup command,
 *            including the color (red, green, blue) and the LEDs that should be set.
 * @param mode The mode in which the function operates. The function performs LED setup
 *             actions when the mode is E_SCHEDULE_MODE_PERFORM.
 * @param flags A pointer to a uint8_t to store additional status or result flags
 *              after executing the LED setup. The function initializes the flags to 0.
 */
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

/**
 * Initializes the LED command functionality by registering command callbacks
 * and configuring the LED strip settings.
 *
 * This function registers two LED-related command callbacks: one for setting up
 * the LED configurations and another for flushing these configurations to the LED strip.
 * It sets the GPIO number and maximum number of LEDs for the LED strip, then creates
 * a new remoter device for the LED strip with a specified RMT configuration.
 *
 * The RMT configuration includes parameters such as resolution and DMA usage.
 * The setup ensures that the LED strip is correctly initialized and ready to handle
 * incoming LED commands.
 */
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


/**
 * This function constructs an LED command setup for a single LED with specified color values.
 * It initializes the LEDs to be updated by building a 32-bit integer representation of the LED number.
 * The command setup is finalized by invoking another function to handle the specific list of LEDs and colors.
 *
 * @param cmd A pointer to the S_command structure for holding the LED command setup.
 * @param LED_number The index of the LED to set up.
 * @param red The intensity value for the red color component (0-255).
 * @param green The intensity value for the green color component (0-255).
 * @param blue The intensity value for the blue color component (0-255).
 * @return A uint8_t value indicating the success or failure of creating the LED command setup.
 */
uint8_t robokit_make_led_command_setup(S_command *cmd, uint8_t LED_number, uint8_t red, uint8_t green, uint8_t blue) {
 	uint32_t leds = 0;
	ROBOKIT_MAKE_LED_LIST(leds, LED_number);

	return robokit_make_led_command_setup_list(cmd, leds, red, green, blue);
}

/**
 * Initializes an LED setup command with specified LED numbers and color values.
 * This function prepares the command structure to control the color of specified LEDs.
 *
 * @param cmd A pointer to the S_command structure that will be initialized for the LED setup command.
 *            The function must successfully cast this to an _S_Command_LED type.
 * @param LED_numbers A 32-bit unsigned integer representing the bitmask of LED numbers to set.
 *                    Each bit corresponds to a specific LED in the strip.
 * @param red A uint8_t value representing the intensity of the red color component (0-255).
 * @param green A uint8_t value representing the intensity of the green color component (0-255).
 * @param blue A uint8_t value representing the intensity of the blue color component (0-255).
 * @return uint8_t Returns 1 if the command was successfully set up, or 0 if the pointer to the command was null.
 */
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

/**
 * Creates a command to flush all the LED setups to the strip.
 * It initializes the command and sets its type to E_COMMAND_LED_FLUSH.
 *
 * @param cmd A pointer to the S_command structure where the LED flush command will be formed.
 *            This structure contains the command type and data to be sent to the LED controller.
 * @return Returns 1 if the command was successfully initialized and formed, or 0 if the input
 *         command pointer is null.
 */
uint8_t robokit_make_led_command_flush(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	cmd->cmd = E_COMMAND_LED_FLUSH;
	cmd->data[0] = 0;

	return 1;
}

/**
 * This function initializes a clear LED command for the Robokit system.
 * It sets up the command structure to flush previously configured LED settings
 * to the LED strip.
 *
 * @param cmd A pointer to the S_command structure that will be initialized for the
 *            LED flush command. This should include the unique command identifier
 *            and any necessary command data.
 *
 * @return Returns 1 if the command structure is successfully initialized and set
 *         for the LED flush operation. Returns 0 if the input command pointer is null.
 */
uint8_t robokit_make_led_command_clear(S_command *cmd) {
	if(!cmd)
		return 0;

	_robokit_command_init(cmd);
	cmd->cmd = E_COMMAND_LED_FLUSH;
	cmd->data[0] = 1;

	return 1;
}