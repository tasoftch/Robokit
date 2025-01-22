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

#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "main_commands.h"

/**
 * The interaction callback function of modules are called by passing one
 * of the following modes to distinguish between checking mode and real,
 * executing mode.
 */
enum {
    // Passed during enqueue.
    // If the precheck fails, the command is not enqueued for the firmware.
    E_SCHEDULE_MODE_PRECHECK = 1,

    // Passed for signalize to execute the
    // command now in the firmware
    E_SCHEDULE_MODE_PERFORM
};

// This function gets called whenever a command gets pushed into the stack.
// In This case the mode is E_SCHEDULE_MODE_PRECHECK.
// In execution time, the function gets called once more with the mode E_SCHEDULE_MODE_PERFORM.
typedef void(*F_command_callback)(S_command *cmd, uint8_t mode, uint8_t *flags);

// If there is any problem in the HW, this function is called.
// It contains an endless loop to hold cpu for debug.
void hal_error_handler(void);

/**
 * @brief This function registers interest of a module on a specified command.
 *
 * @param cb A callback to contact the firmware's abstraction layer
 * @return Returns 1 on success, 0 otherwise
 */
uint8_t robokit_register_command_fn(T_cmd cmd, F_command_callback cb);

#endif //SCHEDULER_H
