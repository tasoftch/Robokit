// Copyright (c) 2024 Th. Abplanalp
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
// documentation files (the “Software”), to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
// and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED
// TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT
// SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
// OR OTHER DEALINGS IN THE SOFTWARE.

//
// Created by Thomas Abplanalp on 15.11.24.
//

#ifndef DRIVE_COMMAND_H
#define DRIVE_COMMAND_H

#include "main_commands.h"

/* DRIVE COMMANDS */
uint8_t robokit_make_drive_command_fwd(S_command *cmd, T_Speed speed);
uint8_t robokit_make_drive_command_bwd(S_command *cmd, T_Speed speed);
uint8_t robokit_make_drive_command_vector(S_command *cmd, S_vector vector);

/**
 * Motor configuration
 * The motor configuration is required to assign motor left and motor right.
 */

typedef struct {
	  uint8_t motor_1:1;
	  uint8_t motor_2:1;
	  uint8_t motor_3:1;
	  uint8_t motor_4:1;
	  uint8_t fast_decay:1;
	  uint8_t switch_direction:1;
} S_motor_config;

// Without setting any configuration, the default is:
// motor3 = left, motor4 = right, both low driven.
void robokit_motor_left_set_config(S_motor_config config);
void robokit_motor_right_set_config(S_motor_config config);

S_motor_config robokit_motor_left_get_config(void);
S_motor_config robokit_motor_right_get_config(void);

uint8_t robokit_drive_command_enable_imu(S_command *cmd);
uint8_t robokit_drive_command_disable_imu(S_command *cmd);

#endif //DRIVE_COMMAND_H
