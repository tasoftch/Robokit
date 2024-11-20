//
//  types.h
//  robokit
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#ifndef types_h
#define types_h


#include <stdint.h>
#include <stdio.h>

// Any speed information is given in percent from 0 to 100.
typedef int8_t T_Speed;
#define ROBOKIT_MIN_SPEED 0
#define ROBOKIT_MAX_SPEED 100

#define ROBOKIT_MAX_SCHEDULED_COMMANDS 32

#define ROBOKIT_DEBUG 1

#endif /* types_h */
