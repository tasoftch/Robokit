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

#include "device.h"
#include "robokit_log.h"
#include "fal.h"


extern void _commands_init(void);
extern void _led_commands_init(void);
extern void _drive_commands_init(void);
extern void _scheduler_init(void);
extern void _test_commands_init(void);
extern void _robokit_pwm_motors_init(void);

/**
 * @brief Init Verteilung in der Hardware
 */
void device_init(void) {
    ROBOKIT_LOGI("Initializing device...");
    _scheduler_init();
    _commands_init();
    _drive_commands_init();
    _led_commands_init();
    _test_commands_init();
    _robokit_pwm_motors_init();
    fal_init();
    ROBOKIT_LOGI("Device initialized");
}

