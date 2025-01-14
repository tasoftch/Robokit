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

#ifndef ROBOKIT_LOG_H
#define ROBOKIT_LOG_H

#include "config.h"
#include <esp_log.h>

static const char *_ROBOKIT_TAG = "HW";

#define ROBOKIT_LOGE( format, ... ) ESP_LOGE(_ROBOKIT_TAG, format, ##__VA_ARGS__)
#define ROBOKIT_LOGW( format, ... ) ESP_LOGW(_ROBOKIT_TAG, format, ##__VA_ARGS__)

#if ROBOKIT_DEBUG
#define ROBOKIT_LOGI( format, ... ) ESP_LOGI(_ROBOKIT_TAG, format, ##__VA_ARGS__)
#define ROBOKIT_LOGD( format, ... ) ESP_LOGD(_ROBOKIT_TAG, format, ##__VA_ARGS__)
#define ROBOKIT_LOGV( format, ... ) ESP_LOGV(_ROBOKIT_TAG, format, ##__VA_ARGS__)
#else
#define ROBOKIT_LOGI( ... )
#define ROBOKIT_LOGD( ... )
#define ROBOKIT_LOGV( ... )
#endif

#endif //ROBOKIT_LOG_H
