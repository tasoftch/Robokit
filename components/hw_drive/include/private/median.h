//
// Created by Thomas Abplanalp on 20.05.25.
//

#ifndef MEDIAN_H
#define MEDIAN_H

#include <stdint.h>

#define ROBOKIT_MEDIAN_FILTER_SIZE 5

typedef struct {
	uint16_t buffer[ROBOKIT_MEDIAN_FILTER_SIZE];
	uint8_t index;
	uint8_t count;
} S_robokit_median_filter_t;

void robokit_median_filter_init(S_robokit_median_filter_t *filter);

void robokit_median_filter_add(S_robokit_median_filter_t *filter, uint16_t value);
uint16_t robokit_median_filter_get(S_robokit_median_filter_t *filter);

#endif //MEDIAN_H
