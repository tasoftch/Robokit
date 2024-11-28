//
// Created by Thomas Abplanalp on 15.11.24.
//



#ifndef LED_COMMAND_H
#define LED_COMMAND_H
#include "main_commands.h"

/* DIE ANZAHL LEDs IST VORLÄUFIG AUF 32 BESCHRÄNKT (4Bytes) */
#define LED_1 0
#define LED_2 1
#define LED_3 2
#define LED_4 3
#define LED_5 4
#define LED_6 5
#define LED_7 6
#define LED_8 7
#define LED_9 8
#define LED_10 9
#define LED_11 10
#define LED_12 11
#define LED_13 12
#define LED_14 13
#define LED_15 14
#define LED_16 15
#define LED_17 16
#define LED_18 17
#define LED_19 18
#define LED_20 19
#define LED_21 20
#define LED_22 21
#define LED_23 22
#define LED_24 23
#define LED_25 24
#define LED_26 25
#define LED_27 26
#define LED_28 27
#define LED_29 28
#define LED_30 29
#define LED_31 30
#define LED_32 31

#define LED_MAX_COUNT 32

/* LED HELPERS */
#define ROBOKIT_LED_COLOR_RED 255, 0, 0
#define ROBOKIT_LED_COLOR_GREEN 0, 255, 0
#define ROBOKIT_LED_COLOR_BLUE 0, 0, 255



#define ROBOKIT_MAKE_LED_LIST(VAR, ...) \
({\
	int __leds[] = {__VA_ARGS__, -1};\
	uint32_t __sum = 0;\
	for(int e=0;e<LED_MAX_COUNT;e++) { if(__leds[e] == -1) break; __sum |= 1 << __leds[e]; }\
	VAR = __sum;\
})


/* LED COMMANDS */
uint8_t robokit_make_led_command_setup(S_command *cmd, uint8_t LED_number, uint8_t red, uint8_t green, uint8_t blue);
uint8_t robokit_make_led_command_setup_list(S_command *cmd, uint32_t LED_numbers, uint8_t red, uint8_t green, uint8_t blue);

uint8_t robokit_make_led_command_flush(S_command *cmd);
uint8_t robokit_make_led_command_clear(S_command *cmd);

#endif //LED_COMMAND_H
