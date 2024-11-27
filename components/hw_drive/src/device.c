//
//  device.c
//  robokit
//
//  Created by Thomas Abplanalp on 07.11.24.
//

#include "device.h"
#include "robokit_log.h"
#include "fal.h"


extern void _commands_init(void);
extern void _led_commands_init(void);
extern void _drive_commands_init(void);
extern void _scheduler_init(void);
extern void _test_commands_init(void);
extern void _robokit_pwm_motors_init(void);

void device_init(void) {
    ROBOKIT_LOGI("Initializing device...");
    _scheduler_init();
    _commands_init();
    _drive_commands_init();
    _led_commands_init();
    _test_commands_init();
    _robokit_pwm_motors_init();
    fal_init();
    ROBOKIT_LOGI("Device initialized");
}

