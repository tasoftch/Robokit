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
};

enum {
	E_COMMAND_FLAG_BLOCK                = 1<<0,
	E_COMMAND_FLAG_WAIT_UNTIL_DONE      = 1<<1,
	E_COMMAND_FLAG_IMU                  = 1<<2
};

enum {
	E_PUSH_STATUS_OK,
	E_PUSH_STATUS_UNKNOWN_COMMAND,
	E_PUSH_STATUS_STACK_FULL
};

typedef struct {
	uint8_t cmd;                // Eindeutige Kennung des Kommandos
	uint8_t flags;              // Eventuelle Optionen mit E_COMMAND_FLAG_* Konstanten
	uint8_t reserved_1;         // Nicht gebraucht, aber vorhanden
	uint8_t reserved_2;         // Nicht gebraucht, aber vorhanden
	union {                     // Speicherung von 4 Bytes in verschiedenen Varianten.
		uint32_t uint32;        // Variante 1: 32 Bit vorzeichenlose Zahl
		int32_t int32;          // Variante 2: 32 Bit Zahl mit Vorzeichen
		struct {                // Variante 3: 4 x 8 Bit Zahlen ohne Vorzeichen
			uint8_t a;
			uint8_t b;
			uint8_t c;
			uint8_t d;
		} uint8;
		struct {                // Variante 4: 4 x 8 Bit Zahlen mit Vorzeichen
			int8_t a;
			int8_t b;
			int8_t c;
			int8_t d;
		} int8;
#if defined(__i386__) || defined(_M_IX86)
		void *ptr;              // Variante 5: Pointer      (nur für 32bit Systeme)
#endif
	} dta;
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
