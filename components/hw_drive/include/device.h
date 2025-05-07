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

#ifndef device_h
#define device_h

#include <stdint.h>
#include "led_command.h"
#include "drive_command.h"
#include "test_commands.h"
#include "main_commands.h"
#include "fal_commands.h"
#include "buzzer_commands.h"
#include "ultrasonic_commands.h"
#include "values.h"

/**
 * @brief Initializes the device components required for operation.
 *
 * This function sets up various subsystems and modules of the device,
 * including the command handlers, scheduler, and PWM motors. It logs
 * the initialization process and subsequently calls the fal initialization.
 */
void device_init(void);

/**
 * @brief Enqueues a command to be processed by the Robokit system.
 *
 * This function pushes a command onto the command stack and schedules it for execution.
 * It performs a precheck to ensure the command is valid and that there is space in the stack.
 *
 * @param[in] cmd S_command* A pointer to the command structure containing the command to be enqueued.
 * @param[in] flags uint8_t Flags indicating specific command behaviors (e.g., block until enqueued).
 *
 * @return uint8_t Status code indicating the result of the operation, such as
 *                 E_PUSH_STATUS_OK, E_PUSH_STATUS_UNKNOWN_COMMAND, E_PUSH_STATUS_STACK_FULL,
 *                 or E_PUSH_PRECHECK_FAILED.
 */
uint8_t robokit_push_command(S_command *cmd, uint8_t flags);

/**
 * @brief Retrieves the number of available spaces in the command queue.
 * @return uint8_t The number of free slots available in the command queue.
 */
uint8_t robokit_get_free_stack_count(void);

/**
 *
 */
void robokit_start_system_indication(void);

/**
*
*/
void robokit_system_ready_indication(void);

/**
 * This function immediately reads the state of the config switch on the device.
 *
* Returns 1 if the system is in configuration mode
*/
uint8_t robokit_system_in_config_mode(void);
#endif /* device_h */
