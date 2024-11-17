//
//  values.h
//  robokit
//
//  Created by Thomas Abplanalp on 07.11.24.
//

#ifndef values_h
#define values_h

#include "types.h"
#include "vector.h"

enum {
    ROBOKIT_MODE_DRIVE_NONE,
    ROBOKIT_MODE_DRIVE_IMU,
    ROBOKIT_MODE_DRIVE_VECTOR,
    ROBOKIT_MODE_DRIVE_FAL,
};
typedef uint8_t T_mode;


#define ROBOKIT_DIRECTION_FORWARD (1)
#define ROBOKIT_DIRECTION_BACKWARD (0)
typedef uint8_t T_direction;

/* DRIVE VALUES */
T_mode robokit_get_current_drive_mode(void);

S_vector robokit_get_current_vector(void);
uint8_t robokit_get_current_speed(void);
T_direction robokit_get_current_direction(void);



/* IMU VALUES */
uint16_t robokit_imu_get_current_acceleration(void);

/* noch nicht sicher, ob das geht.*/
uint16_t robokit_imu_get_current_angle(void);
uint16_t robokit_imu_get_current_position(void);

/* FOLLOW A LINE VALUES */

#define ROBOKIT_FAL_MAX_PATTERN 5

// Es wäre theoretisch möglich, über unterschiedliche Farbmuster Kommandos einzulesen.
// Diese Farbmuster kann ich Dir ausgeben.
// Die Farbmuster sind folgendermassen gedacht: 15 bit <=> 5 Muster
// 000 000 000 000 000
// RGB RGB RGB RGB RGB
// R = G = B = 1 oder 0
// Kommt auf einer Linie etwas anderes als schwarz, wird das im Pattern notiert.

typedef struct {
    uint16_t p0[3];
    uint16_t p1[3];
    uint16_t p2[3];
    uint16_t p3[3];
    uint16_t p4[3];
} S_pattern;

#define ROBOKIT_FAL_PATTERN_BLACK           0b000
#define ROBOKIT_FAL_PATTERN_RED             0b100
#define ROBOKIT_FAL_PATTERN_GREEN           0b010
#define ROBOKIT_FAL_PATTERN_BLUE            0b001
#define ROBOKIT_FAL_PATTERN_YELLOW          0b110
#define ROBOKIT_FAL_PATTERN_MAGENTA         0b101
#define ROBOKIT_FAL_PATTERN_CYAN            0b011
#define ROBOKIT_FAL_PATTERN_WHITE           0b111

S_pattern robokit_fal_get_last_pattern(void);

#endif /* values_h */
