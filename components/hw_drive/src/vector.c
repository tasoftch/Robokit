//
//  vector.c
//  robokit
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#include "vector.h"
#include <math.h>

const S_vector ROBOKIT_INVALID_VECTOR = {-128, -128};

static S_vector _robokit_make_vector_polar(int8_t angle, T_Speed length) {
    if(length > ROBOKIT_MAX_SPEED || length < ROBOKIT_MIN_SPEED)
        return ROBOKIT_INVALID_VECTOR;
    
    S_vector vec = {angle, length};
    return vec;
}

S_vector robokit_make_vector_polar(int16_t angle, T_Speed length) {
    if(length > ROBOKIT_MAX_SPEED || length < ROBOKIT_MIN_SPEED)
        return ROBOKIT_INVALID_VECTOR;
    
    S_vector vec = {angle / 3, length};
    return vec;
}

S_vector robokit_make_vector(int8_t x, int8_t y) {
    float angle = (float)atan2(x, y) * 180.0f / (float)M_PI;
    
    return _robokit_make_vector_polar(
     angle >= 0 ? (angle + 1.5f) / 3.0f : (angle - 1.5f) / 3.0f,
     sqrt( x*x + y*y )
    );
}

uint8_t robokit_vector_equals(S_vector vec1, S_vector vec2) {
    return vec1.angle == vec2.angle && vec1.speed == vec2.speed;
}
