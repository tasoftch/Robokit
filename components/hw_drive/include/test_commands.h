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

#ifndef TEST_COMMANDS_H
#define TEST_COMMANDS_H

#include "main_commands.h"

#define ROBOKIT_USE_BUILTIN_LED 1


/**
 * @brief Creates a test command and updates its current state.
 *
 * Initializes the given command structure and toggles the LED state for testing purposes.
 * The command structure is updated with a test command, and the internal state affects
 * the reserved fields of the command.
 *
 * @param command S_command* Pointer to the command structure that will be initialized and modified.
 * @return Returns 1 if the command structure is successfully initialized and modified, otherwise 0.
 */
uint8_t robokit_make_test_command(S_command *command);

#endif //TEST_COMMANDS_H
