//
// Created by Thomas Abplanalp on 25.05.25.
//

#include "private/parameters.h"
#include <esp_wifi.h>
#include <modules/robokit_module.h>

static struct {
	uint8_t flags;
	union {
		uint8_t u8;
		int8_t i8;
		uint16_t u16;
		int16_t i16;
		uint32_t u32;
		int32_t i32;
		void *ptr;
	} value;
} internal_parameters[ ROBOKIT_MAX_PARAMETERS_COUNT ] = {0};

ROBOKIT_MODULE_INIT() {
	int8_t pwr=0;
	esp_wifi_get_max_tx_power(&pwr);
	internal_parameters[ E_ROBOKIT_PARAM_WLAN_TX_POWER ].value.u8 = pwr;
	internal_parameters[ E_ROBOKIT_PARAM_WLAN_TX_POWER ].flags = E_ROBOKIT_PARAM_TYPE_INTEGER_8 | E_ROBOKIT_PARAM_TYPE_SIGNED;
}

void _robokit_internal_set_parameter(robokit_parameter_name_t name, robokit_parameter_type_t type, uint32_t value) {
	internal_parameters[name].flags = type;
	internal_parameters[name].value.u32 = value;
}

uint32_t _robokit_internal_get_parameter(robokit_parameter_name_t name) {
	return internal_parameters[name].value.u32;
}

uint8_t robokit_parameter_free(robokit_parameter_name_t name) {
	return internal_parameters[name].flags < 1 ? 1 : 0;
}

robokit_parameter_type_t robokit_parameter_type(robokit_parameter_name_t name) {
	return internal_parameters[name].flags;
}

void robokit_parameter_clear(robokit_parameter_name_t name) {
	internal_parameters[name].flags = 0;
	internal_parameters[name].value.u32 = 0;
}

void robokit_parameter_set_ptr(robokit_parameter_name_t name, void *ptr) {
	internal_parameters[name].value.ptr = ptr;
	internal_parameters[name].flags = E_ROBOKIT_PARAM_TYPE_INTEGER_32 | E_ROBOKIT_PARAM_TYPE_POINTER;
}
void *robokit_parameter_get_ptr(robokit_parameter_name_t name) {
	if(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_POINTER) {
		return internal_parameters[name].value.ptr;
	}
	return NULL;
}

void robokit_parameter_set_uint8(robokit_parameter_name_t name, uint8_t value) {
	internal_parameters[name].value.u8 = value;
	internal_parameters[name].flags = E_ROBOKIT_PARAM_TYPE_INTEGER_8;
}
uint8_t robokit_parameter_get_uint8(robokit_parameter_name_t name) {
	if(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_INTEGER_8 && !(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_SIGNED)) {
		return internal_parameters[name].value.u8;
	}
	return ROBOKIT_INVALID_MARKER_8;
}

void robokit_parameter_set_int8(robokit_parameter_name_t name, int8_t value) {
	internal_parameters[name].value.i8 = value;
	internal_parameters[name].flags = E_ROBOKIT_PARAM_TYPE_INTEGER_8 | E_ROBOKIT_PARAM_TYPE_SIGNED;

	if(name == E_ROBOKIT_PARAM_WLAN_TX_POWER) {
		esp_wifi_set_max_tx_power(value);
	}
}
int8_t robokit_parameter_get_int8(robokit_parameter_name_t name) {
	if(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_INTEGER_8 && (internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_SIGNED)) {
		return internal_parameters[name].value.i8;
	}
	return ROBOKIT_INVALID_MARKER_8;
}

void robokit_parameter_set_uint16(robokit_parameter_name_t name, uint16_t value) {
	internal_parameters[name].value.u16 = value;
	internal_parameters[name].flags = E_ROBOKIT_PARAM_TYPE_INTEGER_16;
}
uint16_t robokit_parameter_get_uint16(robokit_parameter_name_t name) {
	if(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_INTEGER_16 && !(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_SIGNED)) {
		return internal_parameters[name].value.u16;
	}
	return ROBOKIT_INVALID_MARKER_16;
}

void robokit_parameter_set_int16(robokit_parameter_name_t name, int16_t value) {
	internal_parameters[name].value.i16 = value;
	internal_parameters[name].flags = E_ROBOKIT_PARAM_TYPE_INTEGER_16 | E_ROBOKIT_PARAM_TYPE_SIGNED;
}
int16_t robokit_parameter_get_int16(robokit_parameter_name_t name) {
	if(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_INTEGER_16 && (internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_SIGNED)) {
		return internal_parameters[name].value.i16;
	}
	return ROBOKIT_INVALID_MARKER_16;
}

void robokit_parameter_set_uint32(robokit_parameter_name_t name, uint32_t value) {
	internal_parameters[name].value.u32 = value;
	internal_parameters[name].flags = E_ROBOKIT_PARAM_TYPE_INTEGER_32;
}
uint32_t robokit_parameter_get_uint32(robokit_parameter_name_t name) {
	if(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_INTEGER_32 && !(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_SIGNED)) {
		return internal_parameters[name].value.u32;
	}
	return ROBOKIT_INVALID_MARKER_32;
}

void robokit_parameter_set_int32(robokit_parameter_name_t name, int32_t value) {
	internal_parameters[name].value.i32 = value;
	internal_parameters[name].flags = E_ROBOKIT_PARAM_TYPE_INTEGER_32 | E_ROBOKIT_PARAM_TYPE_SIGNED;
}
int32_t robokit_parameter_get_int32(robokit_parameter_name_t name) {
	if(internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_INTEGER_32 && (internal_parameters[name].flags & E_ROBOKIT_PARAM_TYPE_SIGNED)) {
		return internal_parameters[name].value.i32;
	}
	return ROBOKIT_INVALID_MARKER_32;
}