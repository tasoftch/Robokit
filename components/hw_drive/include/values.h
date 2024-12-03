/*
 * MIT License
 *
 * Copyright (c) 2024 Th. Abplanalp, F. Romer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef values_h
#define values_h

#include "config.h"
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
