/*
 * MIT License
 *
 * Copyright (c) 2024 Th. Abplanalp, F. Romer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

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
	T_Speed speed;
	uint8_t reserved3;
	void *callback;
} _S_Command_Fal;

#define ROBOKIT_CMD_CAST(TYPE, CMD) ((TYPE)(CMD))

#endif //COMMAND_DATA_CONTAINERS_H
