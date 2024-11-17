//
//  device.c
//  robokit
//
//  Created by Thomas Abplanalp on 07.11.24.
//

#include "device.h"

extern void _commands_init(void);
extern void _led_commands_init(void);
extern void _drive_commands_init(void);
extern void _scheduler_init(void);

void device_init(void) {
    printf("device init\n");
    _scheduler_init();
    _commands_init();
    _drive_commands_init();
    _led_commands_init();
}

