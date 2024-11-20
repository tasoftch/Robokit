//
//  commands.h
//  robokit
//
//  Created by Thomas Abplanalp on 07.11.24.
//

#ifndef commands_h
#define commands_h

#include "types.h"
#include "vector.h"

typedef uint8_t T_cmd;

enum {
	E_COMMAND_NONE = 0,							// None or invalid command

	/* DRIVE COMMANDS */
	E_COMMAND_VECTOR,							// Drive using arg1 as X vector
												// and arg2 as Y vector.

	/* LED COMMANDS */
	E_COMMAND_LED_SETUP,						// Choose a LED number and set its color
	E_COMMAND_LED_FLUSH,						// Sends all LED setups to strip

	E_COMMAND_TEST = ROBOKIT_MAX_SCHEDULED_COMMANDS - 1
};

enum {
	E_COMMAND_FLAG_BLOCK                = 1<<0,		// Wait until command is enqueued
	E_COMMAND_FLAG_WAIT_UNTIL_DONE      = 1<<1,		// Wait until command is done by hardware
	E_COMMAND_FLAG_IMU                  = 1<<2		// Enables IMU
};

enum {
	E_PUSH_STATUS_OK,
	E_PUSH_STATUS_UNKNOWN_COMMAND,
	E_PUSH_STATUS_STACK_FULL
};

typedef struct {
	uint8_t cmd;                // Eindeutige Kennung des Kommandos
	uint8_t data[7];			// Datencontainer
} S_command;


// Using Pointers in commands, please use this functions for proper working.
uint8_t robokit_drive_command_set_pointer(S_command *cmd, void *prt);
void *robokit_drive_command_get_pointer(S_command *cmd);

/* FOLLOW A LINE */
uint8_t robokit_make_drive_command_fal_enable(S_command *cmd, uint8_t speed);
uint8_t robokit_make_drive_command_fal_disable(S_command *cmd);

// noch nicht ganz fertig.
uint8_t robokit_make_drive_command_fal_pattern(S_command *cmd);

#endif /* commands_h */
