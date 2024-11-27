//
// Created by Thomas Abplanalp on 27.11.24.
//

#ifndef FAL_H
#define FAL_H

#define GPIO_RED   10  // GPIO_10 als Ausgang für Rot
#define GPIO_GREEN 11  // GPIO_11 als Ausgang für Grün
#define GPIO_BLUE  12  // GPIO_12 als Ausgang für Blau

#include <stdint.h>


typedef struct {
	uint8_t fb_1_left;
	uint8_t fb_2_middle_left;
	uint8_t fb_3_middle;
	uint8_t fb_4_middle_right;
	uint8_t fb_5_right;
} S_Fal_Result;


void fal_init(void);

uint8_t fal_is_calibrated(void);

void fal_update(void);

#endif //FAL_H
