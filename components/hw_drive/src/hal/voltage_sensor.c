//
// Created by Thomas Abplanalp on 08.05.25.
//
#include "hal/voltage_sensor.h"
#include <driver/adc.h>
#include "modules/robokit_module.h"

#define ROBOKIT_VOLTAGE_SENSOR ADC_CHANNEL_5
#define ROBOKIT_VOLTAGE_SENSOR_TIMER_INTERVAL 10

static volatile uint16_t voltage = 0;


ROBOKIT_MODULE_INIT() {
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ROBOKIT_VOLTAGE_SENSOR, ADC_ATTEN_DB_12);
}

ROBOKIT_MODULE_SENSOR_LOOP() {
	static uint8_t timer = ROBOKIT_VOLTAGE_SENSOR_TIMER_INTERVAL;

	if(timer-- < 1) {
		timer = ROBOKIT_VOLTAGE_SENSOR_TIMER_INTERVAL;
		uint32_t value = adc1_get_raw(ROBOKIT_VOLTAGE_SENSOR);
		value *= 5002;
		voltage += value / 2289;
		voltage /= 2;
	}
}