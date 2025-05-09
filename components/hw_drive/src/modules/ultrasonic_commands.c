//
// Created by Thomas Abplanalp on 06.05.25.
//

#include "ultrasonic_commands.h"

#include <stdio.h>
#include <hal/hc_sr04.h>
#include <modules/robokit_module.h>

typedef struct {
	T_cmd cmd;
	uint8_t reserve1;
	uint16_t distance;
	uint8_t reserve2;
	uint8_t reserve3;
	uint8_t reserve4;
	uint8_t reserve5;
} _S_command_US;

ROBOKIT_CHECK_COMMAND_STRUCT(_S_command_US);

static uint8_t ultrasonic_status = 0;
static uint16_t ultrasonic_distance = 0;
static uint16_t ultrasonic_sensor_measurement = 0;

#define ULTRASONIC_TIMER_STAT 5

robokit_distance_cm_t robokit_ultrasonic_get_distance(void) {
	return ultrasonic_sensor_measurement;
}

ROBOKIT_MODULE_INIT() {
	hc_sr04_init();
}

ROBOKIT_MODULE_COMMAND_HANDLER(E_COMMAND_ULTRASONIC, _S_command_US *cmd, uint8_t mode, uint8_t *flags) {
	if (mode == E_SCHEDULE_MODE_PERFORM) {
		if(cmd->distance > 10) {
			ultrasonic_status = 1;
			ultrasonic_distance = cmd->distance;
		}
	}
}


ROBOKIT_MODULE_SENSOR_LOOP() {
	static uint8_t intern_timer = ULTRASONIC_TIMER_STAT;

	if(intern_timer-- < 1) {
		intern_timer = ULTRASONIC_TIMER_STAT;
		ultrasonic_sensor_measurement = hc_sr04_read_distance_cm();
		if(ultrasonic_status) {
			if(ultrasonic_sensor_measurement < ultrasonic_distance) {
				S_command cmd;
				robokit_make_drive_command_fwd(&cmd, 0);
				robokit_push_command(&cmd, 0);
				intern_timer = ULTRASONIC_TIMER_STAT;
				ultrasonic_status = 0;
			}
		}
	}
}



uint8_t robokit_make_command_us_stop_at_distance(S_command *cmd, uint16_t distance_cm) {
	if(!cmd)
		return 0;

	ROBOKIT_COMMAND_RESET_P(cmd);
	_S_command_US *us = (_S_command_US *)cmd;
	us->cmd = E_COMMAND_ULTRASONIC;
	us->distance = distance_cm;

	return 1;
}