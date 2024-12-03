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

#ifndef n_lut_h
#define n_lut_h

#include <stdio.h>

static const int8_t _motor_lut_pwm_negative_deg[120][2] = {
    {100, 100},
    {99, 100},
    {98, 100},
    {95, 100},
    {91, 100},
    {87, 100},
    {81, 100},
    {74, 100},
    {67, 100},
    {59, 100},
    {50, 100},
    {41, 100},
    {31, 100},
    {21, 100},
    {10, 100},
    {0, 100},
    {-10, 100},
    {-21, 100},
    {-31, 100},
    {-41, 100},
    {-50, 100},
    {-59, 100},
    {-67, 100},
    {-74, 100},
    {-81, 100},
    {-87, 100},
    {-91, 100},
    {-95, 100},
    {-98, 100},
    {-99, 100},
    {-100, 100},
    {-100, 99},
    {-100, 98},
    {-100, 95},
    {-100, 91},
    {-100, 87},
    {-100, 81},
    {-100, 74},
    {-100, 67},
    {-100, 59},
    {-100, 50},
    {-100, 41},
    {-100, 31},
    {-100, 21},
    {-100, 10},
    {-100, 0},
    {-100, -10},
    {-100, -21},
    {-100, -31},
    {-100, -41},
    {-100, -50},
    {-100, -59},
    {-100, -67},
    {-100, -74},
    {-100, -81},
    {-100, -87},
    {-100, -91},
    {-100, -95},
    {-100, -98},
    {-100, -99},
    {-100, -100}
};

#endif /* n_lut_h */
