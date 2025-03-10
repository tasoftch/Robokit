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

#ifndef types_h
#define types_h

#include <stdint.h>

// Die Angaben zu einer Geschwindigkeit sind immer in Prozent von 0% bis 100%
typedef int8_t T_Speed;

// Diese Direktiven definieren die Limits.
#define ROBOKIT_MIN_SPEED 0
#define ROBOKIT_MAX_SPEED 100

// Definition, wie viele Kommandos im Scheduler registriert werden können.
#define ROBOKIT_MAX_SCHEDULED_COMMANDS 32

// Kommando-Stacksize für push_command Aufrufe
#define ROBOKIT_COMMAND_STACK_SIZE 8

// Debug Modus. Alle ROBOKIT_LOGX() Makros werden ausgeschaltet, wenn Debug=0
#define ROBOKIT_DEBUG 1

#endif /* types_h */
