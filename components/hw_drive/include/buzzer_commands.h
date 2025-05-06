//
// Created by Thomas Abplanalp on 05.05.25.
//

#ifndef BUZZER_COMMANDS_H
#define BUZZER_COMMANDS_H

#include <main_commands.h>

uint8_t robokit_make_command_buzzer_beep(S_command *cmd, uint16_t freq, uint16_t duration_ms);
uint8_t robokit_make_command_buzzer_mute(S_command *cmd);

#endif //BUZZER_COMMANDS_H
