//
//  device.h
//  robokit
//
//  Created by Thomas Abplanalp on 07.11.24.
//

#ifndef device_h
#define device_h

#include <stdint.h>
#include "led_command.h"
#include "drive_command.h"
#include "test_commands.h"
#include "main_commands.h"
#include "fal_commands.h"
#include "values.h"

// Bitte diese Funktion in main aufrufen.
// Sie initialisiert alle Hardware und Firmware.
void device_init(void);

// Danach das Kommando absetzen und gut ist's.
// Flags werden folgende sein:
// Flag für warten, wenn der Command Stack voll ist.
// Flag für warten, bis das Kommando ausgeführt wurde.
// ... falls noch weitere notwendig sind.
uint8_t robokit_push_command(S_command *cmd, uint8_t flags);

/* Returns how many commands can be added set. */
uint8_t robokit_get_free_stack_count(void);

#endif /* device_h */
