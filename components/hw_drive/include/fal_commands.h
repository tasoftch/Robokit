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

#ifndef FAL_COMMANDS_H
#define FAL_COMMANDS_H

#include "main_commands.h"

#define ROBOKIT_FAL_BLACK			0b000
#define ROBOKIT_FAL_RED				0b100
#define ROBOKIT_FAL_GREEN			0b010
#define ROBOKIT_FAL_BLUE			0b001
#define ROBOKIT_FAL_YELLOW			0b110
#define ROBOKIT_FAL_MAGENTA			0b101
#define ROBOKIT_FAL_CYAN			0b011
#define ROBOKIT_FAL_WHITE			0b111

typedef struct {
 uint16_t fb_1_left:3;
 uint16_t fb_2_middle_left:3;
 uint16_t fb_3_middle:3;
 uint16_t fb_4_middle_right:3;
 uint16_t fb_5_right:3;
} S_Fal_Result;

typedef void (*Fal_Interpreter_Callback)(S_Fal_Result *result);

/**
 * @brief Constructs a FAL (Follow A Line) calibrate command.
 * @param[out] cmd S_command* Pointer to the command structure to be initialized.
 * @param[in] calibrated void(*)(uint8_t) Function pointer for the calibration callback.
 * @return uint8_t Returns 1 if the command is successfully constructed, 0 otherwise.
 */
uint8_t robokit_make_command_fal_calibrate(S_command *cmd, void (*calibrated)(uint8_t));

/**
 * @brief Initializes and enables a follow a line (FAL) command.
 *
 * This function initializes the given command structure to prepare it
 * for enabling the follow a line function. The command is set to
 * E_COMMAND_FAL and the option is set to enable.
 *
 * @param[out] cmd S_command* Pointer to the command structure to initialize and enable.
 * @return uint8_t Returns 1 if the operation is successful, or 0 if the cmd pointer is null.
 */
uint8_t robokit_make_command_fal_enable(S_command *cmd);

/**
 * @brief Disables the Follow A Line (FAL) sensor command.
 *
 * This function initializes and configures the command structure `cmd`
 * to disable the Follow A Line (FAL) sensor. It sets the appropriate
 * command and flags within the structure.
 *
 * @param [out] cmd S_command* Pointer to the command structure to be configured.
 * @return uint8_t Returns 1 if the command was successfully configured, 0 if the input pointer was null.
 */
uint8_t robokit_make_command_fal_disable(S_command *cmd);

/**
 * @brief Sets the line result interpreter function.
 *
 * This function gets frequently called upon the follow a line sensors measurement result.
 * It is responsible to translate the result into drive commands for adjusting the direction.
 *
 * @param [in] interpreter void(*)(S_Fal_Result*) Function pointer to be used as the new line result interpreter.
 */
void fal_set_line_result_interpreter(Fal_Interpreter_Callback interpreter);


/**
 * @brief Interprets the line following sensor result and issues appropriate drive commands.
 * @param [in] result S_Fal_Result* Pointer to the sensor data structure containing readings from five sensors.
 */
void fal_default_line_result_interpreter(S_Fal_Result *result);


#endif //FAL_COMMANDS_H
