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
#include "values.h"

// Bitte diese Funktion in main aufrufen.
// Sie initialisiert alle Hardware und Firmware.
void device_init(void);

// Danach das Kommando absetzen und gut ist's.
// Flags werden folgende sein:
// Flag für warten, wenn der Command Stack voll ist.
// Flag für warten, bis das Kommando ausgeführt wurde.
// ... falls noch weitere notwendig sind.
uint8_t robokit_push_command(S_command *cmd, uint8_t flags);

/* Returns how many commands can be added set. */
uint8_t robokit_get_free_stack_count(void);

#endif /* device_h */
