//
// Created by Thomas Abplanalp on 27.11.24.
//

#ifndef FAL_COMMANDS_H
#define FAL_COMMANDS_H

#include "main_commands.h"

uint8_t robokit_make_command_fal_calibrate(S_command *cmd, void (*calibrated)(uint8_t));

uint8_t robokit_make_command_fal_enable(S_command *cmd);
uint8_t robokit_make_command_fal_disable(S_command *cmd);

#endif //FAL_COMMANDS_H
