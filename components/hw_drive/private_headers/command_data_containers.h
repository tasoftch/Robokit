//
// Created by Thomas Abplanalp on 18.11.24.
//

#ifndef COMMAND_DATA_CONTAINERS_H
#define COMMAND_DATA_CONTAINERS_H

#include "main_commands.h"
#include "drive_command.h"
#include "motor_logic.h"

typedef struct {
	T_cmd command;
   	uint8_t flags;
	T_Speed speed;
	int8_t angle;
    S_motor_config m_left;
    S_motor_config m_right;
    S_motor_ctl m_ctl_left;
    S_motor_ctl m_ctl_right;
} _S_command_drive;

typedef struct {
	T_cmd cmd;
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint32_t LEDs;
} _S_Command_LED;

typedef struct {
	T_cmd cmd;
	uint8_t reserved1;
	uint8_t reserved2;
	uint8_t reserved3;
	uint32_t reseerved4;
} _S_Command_test;

typedef struct {
	T_cmd cmd;
	uint8_t flags;
	uint8_t reserved2;
	uint8_t reserved3;
	void *callback;
} _S_Command_Fal;

#define _ROBOKIT_CMD_CAST(TYPE, CMD) ((TYPE)(CMD))

#endif //COMMAND_DATA_CONTAINERS_H
