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

/**
 * @brief The FAL_Result structure holds the color for each sensor cell.
 * Please note that the cells are assigned from left to right in forward drive direction.
 *
 * In pro3E, the FAL_Result has only black or white colors available.
 */
typedef struct {
    uint16_t fb_1_left:3;
    uint16_t fb_2_middle_left:3;
    uint16_t fb_3_middle:3;
    uint16_t fb_4_middle_right:3;
    uint16_t fb_5_right:3;
} S_Fal_Result;


enum {
 ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED = 0,
 ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED = 1
};
typedef uint8_t robokit_fal_interpreter_result_t;

// A follow a line interpreter function.
// The function must interpret the results from follow a line sensor.
// If the function accepts the result, it must return 1, otherwise 0.
// The follow a line concept stops the drive if too long no result was interpreted.
typedef robokit_fal_interpreter_result_t (*Fal_Interpreter_Callback)(S_Fal_Result *result, T_Speed speed);



/**
 * @brief Constructs a FAL (Follow A Line) calibration command.
 *
 * The system assumes, that the drive will pass a black line that fully covers all sensor cells.
 *
 * For the calibration, the follow a line system will reset the current color references and enqueues
 * a forward drive command with a given speed (shall be 50%).
 * It will capture the maximum and minimum values of each sensor cell for a maximum time of 1s.
 * As soon as there is a difference, it will calculate a threshold and sends a stop command.
 * Depending, if timeout passed or the threshold could be calculated it will call the callback function with 1
 * as parameter, if the calibration was successful or 0 otherwise.
 *
 * @param cmd A Pointer to the command structure to be initialized.
 * @param speed A speed constant to specify the forward speed for calibration
 * @param timeout A timeout in ms. Please note that the accuracy of timeout is only 40ms. Max 10s!
 * @param calibrated A Function pointer for the calibration callback.
 * @return Returns 1 if the command was successfully configured, 0 if the input pointer was null.
 */
uint8_t robokit_make_command_fal_calibrate(S_command *cmd, T_Speed speed, uint16_t timeout, void (*calibrated)(uint8_t));

/**
 * @brief Constructs a drive forward command using the follow a line (FAL) sensor.
 *
 * This function initializes the given command structure to prepare it
 * for driving along a black line. If the sensor does not recognize a black line anymore,
 * it will send a stop command.
 *
 * @param cmd A Pointer to the command structure to be initialized.
 * @param timeout A timeout in ms. Please note that the accuracy of timeout is only 40ms. Max 10s!
 * @return Returns 1 if the command was successfully configured, 0 if the input pointer was null.
 */
uint8_t robokit_make_command_fal_drive(S_command *cmd, T_Speed speed, uint16_t timeout);

/**
* Just enables the follow a line sensor to obtain background color information.
*/
uint8_t robokit_make_command_fal_enable(S_command *cmd);

/**
 * @brief Constructs a stop immediately command disabling the follow a line (FAL) sensor.
 *
 * This function initializes and configures the command structure `cmd`
 * to disable the Follow A Line (FAL) sensor. Once enqueued, it will also immediately stop the drive.
 *
 * @param cmd A Pointer to the command structure to be configured.
 * @return Returns 1 if the command was successfully configured, 0 if the input pointer was null.
 */
uint8_t robokit_make_command_fal_disable(S_command *cmd);

/**
* Makes
*/
uint8_t robokit_make_command_fal_one_shot(S_command *cmd, void(*complete_handler)(void));


/**
 * @brief Sets the line result interpreter function.
 *
 * This function gets frequently called upon the follow a line sensors measurement result.
 * It is responsible to translate the result into drive commands for adjusting the direction.
 *
 * @param interpreter A Function pointer to transform follow a line results into drive commands.
 */
void fal_set_line_result_interpreter(Fal_Interpreter_Callback interpreter);


/**
 * @brief Interprets the line following sensor result and issues default drive commands.
 *
 * @param result A Pointer to the sensor data structure containing readings from five sensors.
 */
robokit_fal_interpreter_result_t fal_default_line_result_interpreter(S_Fal_Result *result, T_Speed speed);


#endif //FAL_COMMANDS_H
