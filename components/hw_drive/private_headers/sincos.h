//
//  sincos.h
//  robokit
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#ifndef sincos_h
#define sincos_h

#include <stdio.h>
#include <stdint.h>

/**
 Returns sine in 6 degree steps of -360 until +360 degree angle in percent 0 - 100
 rounds half up
 */
int8_t robokit_sin(int16_t degree);
int8_t robokit_cos(int16_t degree);

void robokit_sincos(int16_t degree, int8_t *sine, int8_t *cosine);

#endif /* sincos_h */
