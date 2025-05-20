//
// Created by Thomas Abplanalp on 20.05.25.
//
#include "private/median.h"

void robokit_median_filter_init(S_robokit_median_filter_t *filter) {
	for(int e=0;e < ROBOKIT_MEDIAN_FILTER_SIZE;e++) {
		filter->buffer[e] = 0;
	}
	filter->count = filter->index = 0;
}

void robokit_median_filter_add(S_robokit_median_filter_t *filter, uint16_t value) {
	filter->buffer[filter->index] = value;
	filter->index = (filter->index + 1) % ROBOKIT_MEDIAN_FILTER_SIZE;
	if (filter->count < ROBOKIT_MEDIAN_FILTER_SIZE) filter->count++;
}


static void robokit_median_filter_sort_copy(const uint16_t* src, uint16_t* dst, uint8_t len) {
	for (int i = 0; i < len; i++) dst[i] = src[i];

	for (int i = 0; i < len - 1; i++) {
		for (int j = 0; j < len - 1 - i; j++) {
			if (dst[j] > dst[j+1]) {
				int tmp = dst[j];
				dst[j] = dst[j+1];
				dst[j+1] = tmp;
			}
		}
	}
}


uint16_t robokit_median_filter_get(S_robokit_median_filter_t *filter) {
	uint16_t sorted[ROBOKIT_MEDIAN_FILTER_SIZE];
	robokit_median_filter_sort_copy(filter->buffer, sorted, filter->count);
	return sorted[filter->count / 2];
}