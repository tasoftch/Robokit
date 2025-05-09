//
// Created by Thomas Abplanalp on 15.04.25.
//

#ifndef HC_SR04_H
#define HC_SR04_H

#include <stdint.h>

void hc_sr04_init();
uint16_t hc_sr04_read_distance_cm();

#endif //HC_SR04_H
