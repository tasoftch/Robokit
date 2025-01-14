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

//
//  commands.c
//  robokit
//
//  Created by Thomas Abplanalp on 07.11.24.
//

#include "main_commands.h"
#include <private/robokit_log.h>

/**
 * Initializes the given command structure to zero.
 *
 * This function sets all fields of the provided S_command structure
 * to their default or initial state by zero-initializing the structure.
 *
 * @param cmd Pointer to the S_command structure to be initialized.
 *            If the pointer is null, the function does nothing.
 */
void _robokit_command_init(S_command *cmd) {
    if(cmd) {
        *cmd = ((S_command){0});
    }
}

/**
 * Initializes the command system for the application.
 *
 * This function logs an informational message indicating that the
 * command system initialization process has started. It is intended
 * to be called during the device initialization sequence.
 */
void _commands_init(void) {
    ROBOKIT_LOGI("Commands init");
}