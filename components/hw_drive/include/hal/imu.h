//
// Created by Thomas Abplanalp on 11.03.25.
//

#ifndef IMU_H
#define IMU_H

#include <esp_err.h>

#define IMU_FILTER_DIR_THRESHOLD 10
#define IMU_FILTER_DIR_TRUST_THRESHOLD 30

typedef enum {
	E_IMU_STATUS_FLAG_NONE = 0,
	E_IMU_STATUS_FLAG_ENABLED = 1<<0,
	E_IMU_STATUS_FLAG_CALIBRATION = 1<<1,
} E_imu_status;

#endif //IMU_H
