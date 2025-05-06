//
// Created by Thomas Abplanalp on 06.05.25.
//

#ifndef ULTRASONIC_SENSOR_COMMANDS_H
#define ULTRASONIC_SENSOR_COMMANDS_H

#include "main_commands.h"


uint8_t robokit_make_command_us_stop_at_distance(S_command *cmd, uint8_t distance_cm);


#endif //ULTRASONIC_SENSOR_COMMANDS_H
