//
// Created by Thomas Abplanalp on 18.11.24.
//

#ifndef TEST_COMMANDS_H
#define TEST_COMMANDS_H

#include "main_commands.h"

#define ROBOKIT_USE_BUILTIN_LED 1


// The test command just blinks the LED on board
// at Pin 8. This pin is also wired to GPIO of the drive.
// The LED on board is the first one of a serie of WS2812B LEDs
uint8_t robokit_make_test_command(S_command *command);

#endif //TEST_COMMANDS_H
