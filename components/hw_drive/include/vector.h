//
//  vector.h
//  robokit
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#ifndef vector_h
#define vector_h

#include "types.h"

typedef struct {
    int8_t angle;          // In degree / 3
    T_Speed speed;
} S_vector;

extern const S_vector ROBOKIT_INVALID_VECTOR;

#define ROBOKIT_MAKE_VECTOR(ANGLE, SPEED) ((S_vector){(ANGLE), (SPEED)})

// === DIESE FUNKTIONEN WERDEN AUSSERHALB DER HW BENUTZT. ==
// Die enthalten die math.h Library um Winkel und Geschwindigkeiten zu berechnen.

// Vektor erstellen aus kartesischen Angaben.
// Achtung!
// Der Vektor darf in keine Richtung schneller sein als ROBOKIT_MAX_SPEED.
// Also: sqrt(left_right^2 + forward_backward^2) <= ROBOKIT_MAX_SPEED!
// Die Funktion wird ROBOKIT_INVALID_VECTOR zurückgeben, wenn sich der Vektor ausserhalb der Geschwindigkeit befindet.
// left_right [-100 .. +100] => left = -100, right = +100
// forward_backward [+100 .. -100] forward = +100, backward = -100
S_vector robokit_make_vector(int8_t left_right, int8_t forward_backward);

// Vektor berechnen lassen aus Richtung und Geschwindigkeit:
// Richtung 0 => geradeaus
// 0 .. +90  => nach rechts abdrehen (Motor rechts wird langsamer. Bei 90° bleibt Motor rechts stehen)
// 90 .. 180 => Motor rechts beginnt rückwärts zu laufen (180 => Fahrzeug dreht um eigene Achse)
// Dasselbe auf die andere Seite 0 .. -180
// Length ist die Geschwindigkeit und reicht von 0 - 100
S_vector robokit_make_vector_polar(int16_t angle, T_Speed speed);

// Prüfe, ob zwei Vektoren gleich sind.
uint8_t robokit_vector_equals(S_vector vec1, S_vector vec2);

#endif /* vector_h */
