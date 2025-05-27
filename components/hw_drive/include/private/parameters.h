//
// Created by Thomas Abplanalp on 25.05.25.
//

#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <stdint.h>

#define ROBOKIT_MAX_PARAMETERS_COUNT 8

#define ROBOKIT_INVALID_MARKER_8 0xFF
#define ROBOKIT_INVALID_MARKER_16 0xFFFF
#define ROBOKIT_INVALID_MARKER_32 0xFFFFFFFF

enum {
	E_ROBOKIT_PARAM_TYPE_NONE				= 0,
	E_ROBOKIT_PARAM_TYPE_INTEGER_8			= 1<<0,
	E_ROBOKIT_PARAM_TYPE_INTEGER_16			= 1<<1,
	E_ROBOKIT_PARAM_TYPE_INTEGER_32			= 1<<2,
	E_ROBOKIT_PARAM_TYPE_SIGNED				= 1<<7,

	E_ROBOKIT_PARAM_TYPE_POINTER			= 1<<6,
};
typedef uint8_t robokit_parameter_type_t;

enum {
	E_ROBOKIT_PARAM_WLAN_TX_POWER			= 0x00,
	E_ROBOKIT_PARAM_FAL_THRESHOLD			= 0x01
};
typedef uint8_t robokit_parameter_name_t;

uint8_t robokit_parameter_free(robokit_parameter_name_t name);
robokit_parameter_type_t robokit_parameter_type(robokit_parameter_name_t name);
void robokit_parameter_clear(robokit_parameter_name_t name);

void robokit_parameter_set_ptr(robokit_parameter_name_t name, void *ptr);
void *robokit_parameter_get_ptr(robokit_parameter_name_t name);

void robokit_parameter_set_uint8(robokit_parameter_name_t name, uint8_t value);
uint8_t robokit_parameter_get_uint8(robokit_parameter_name_t name);

void robokit_parameter_set_int8(robokit_parameter_name_t name, int8_t value);
int8_t robokit_parameter_get_int8(robokit_parameter_name_t name);

void robokit_parameter_set_uint16(robokit_parameter_name_t name, uint16_t value);
uint16_t robokit_parameter_get_uint16(robokit_parameter_name_t name);

void robokit_parameter_set_int16(robokit_parameter_name_t name, int16_t value);
int16_t robokit_parameter_get_int16(robokit_parameter_name_t name);

void robokit_parameter_set_uint32(robokit_parameter_name_t name, uint32_t value);
uint32_t robokit_parameter_get_uint32(robokit_parameter_name_t name);

void robokit_parameter_set_int32(robokit_parameter_name_t name, int32_t value);
int32_t robokit_parameter_get_int32(robokit_parameter_name_t name);

#endif //PARAMETERS_H
