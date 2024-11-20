//
// Created by Thomas Abplanalp on 20.11.24.
//

#ifndef ROBOKIT_LOG_H
#define ROBOKIT_LOG_H

#include "config.h"

static const char *_ROBOKIT_TAG = "HW";

#if ROBOKIT_DEBUG
#define ROBOKIT_LOGE( format, ... ) ESP_LOGE(_ROBOKIT_TAG, format, ##__VA_ARGS__)
#define ROBOKIT_LOGW( format, ... ) ESP_LOGW(_ROBOKIT_TAG, format, ##__VA_ARGS__)
#define ROBOKIT_LOGI( format, ... ) ESP_LOGI(_ROBOKIT_TAG, format, ##__VA_ARGS__)
#define ROBOKIT_LOGD( format, ... ) ESP_LOGD(_ROBOKIT_TAG, format, ##__VA_ARGS__)
#define ROBOKIT_LOGV( format, ... ) ESP_LOGV(_ROBOKIT_TAG, format, ##__VA_ARGS__)
#else
#define ROBOKIT_LOGE( ... )
#define ROBOKIT_LOGW( ... )
#define ROBOKIT_LOGI( ... )
#define ROBOKIT_LOGD( ... )
#define ROBOKIT_LOGV( ... )
#endif

#define ROBOKIT_LOGI()

#endif //ROBOKIT_LOG_H
