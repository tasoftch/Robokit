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

#ifndef LED_COMMAND_H
#define LED_COMMAND_H
#include "main_commands.h"

/* DIE ANZAHL LEDs IST VORLÄUFIG AUF 32 BESCHRÄNKT (4Bytes) */
#define LED_1 0
#define LED_2 1
#define LED_3 2
#define LED_4 3
#define LED_5 4
#define LED_6 5
#define LED_7 6
#define LED_8 7
#define LED_9 8
#define LED_10 9
#define LED_11 10
#define LED_12 11
#define LED_13 12
#define LED_14 13
#define LED_15 14
#define LED_16 15
#define LED_17 16
#define LED_18 17
#define LED_19 18
#define LED_20 19
#define LED_21 20
#define LED_22 21
#define LED_23 22
#define LED_24 23
#define LED_25 24
#define LED_26 25
#define LED_27 26
#define LED_28 27
#define LED_29 28
#define LED_30 29
#define LED_31 30
#define LED_32 31

#define LED_MAX_COUNT 32

/* LED HELPERS */
#define ROBOKIT_LED_COLOR_RED 255, 0, 0
#define ROBOKIT_LED_COLOR_GREEN 0, 255, 0
#define ROBOKIT_LED_COLOR_BLUE 0, 0, 255



#define ROBOKIT_MAKE_LED_LIST(VAR, ...) \
({\
	int __leds[] = {__VA_ARGS__, -1};\
	uint32_t __sum = 0;\
	for(int e=0;e<LED_MAX_COUNT;e++) { if(__leds[e] == -1) break; __sum |= 1 << __leds[e]; }\
	VAR = __sum;\
})


/**
 * @brief Creates a command to set up a specific LED with the given color parameters.
 *
 * This function sets up the command structure to configure a specific LED number with
 * designated red, green, and blue color intensity values.
 *
 * @param [out] cmd S_command* Pointer to the command structure that will be populated.
 * @param [in] LED_number uint8_t The LED number to configure.
 * @param [in] red uint8_t Intensity of the red color component.
 * @param [in] green uint8_t Intensity of the green color component.
 * @param [in] blue uint8_t Intensity of the blue color component.
 * @return uint8_t Result code indicating success or failure of the command setup.
 */
uint8_t robokit_make_led_command_setup(S_command *cmd, uint8_t LED_number, uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Configures a list of LEDs with specified color values.
 *
 * This function prepares the LED command by setting the LED number and its desired
 * RGB color values in the provided command structure. It initializes the command and
 * assigns the appropriate command type for LED setup.
 *
 * @param[out] cmd S_command* A pointer to the command structure to be configured.
 * @param[in] LED_numbers uint32_t A bit mask representing the LED numbers to be configured.
 * @param[in] red uint8_t The intensity of the red color (0-255) for the LEDs.
 * @param[in] green uint8_t The intensity of the green color (0-255) for the LEDs.
 * @param[in] blue uint8_t The intensity of the blue color (0-255) for the LEDs.
 *
 * @return uint8_t Returns 1 on successful configuration, 0 if the command structure is null.
 */
uint8_t robokit_make_led_command_setup_list(S_command *cmd, uint32_t LED_numbers, uint8_t red, uint8_t green, uint8_t blue);

/**
 * @brief Prepares a command to flush LED settings.
 *
 * This function initializes the given command structure for flushing LED settings
 * and sets the appropriate command and data values.
 *
 * @param [out] cmd S_command* Pointer to the command structure to be initialized and set for LED flush.
 * @return uint8_t Returns 1 if the command was successfully prepared, otherwise returns 0.
 */
uint8_t robokit_make_led_command_flush(S_command *cmd);

/**
 * @brief Initializes the LED command to clear the current state.
 *
 * This function prepares a command structure to flush all LED setups.
 * It initializes the command structure with the necessary settings to clear the LED state.
 *
 * @param [out] cmd S_command* Pointer to the command structure to be initialized.
 * @return uint8_t Returns 1 if the command structure is successfully initialized, 0 otherwise.
 */
uint8_t robokit_make_led_command_clear(S_command *cmd);

#endif //LED_COMMAND_H
