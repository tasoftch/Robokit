//
// Created by Thomas Abplanalp on 25.03.25.
//

#ifndef FILTER_H
#define FILTER_H

#include <stdint.h>

typedef struct {
	uint8_t mode;
	uint8_t data[7];
} S_filter;

void filter_init(S_filter *filter);

void filter_angle_init(S_filter *filter, uint8_t deviation_threshold, uint8_t trust_threshold);

void filter_angle_put_reference(S_filter *filter);
void filter_angle_deviate_reference(S_filter *filter, int8_t deviation);

void filter_angle_dereference(S_filter *filter);
int16_t filter_angle_get_reference(S_filter *filter);

void filter_angle_put_value(S_filter *filter, int16_t angle);
int16_t filter_angle_get_value(S_filter *filter);

#endif //FILTER_H
