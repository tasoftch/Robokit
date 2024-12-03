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

#ifndef vector_h
#define vector_h

#include "config.h"

typedef struct {
    int8_t angle;          // In degree / 3
    T_Speed speed;
} S_vector;

extern const S_vector ROBOKIT_INVALID_VECTOR;

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
// Wird der Winkel grösser als 0 Grad, beginnt das Fahrzeut nach rechts abzudrehen.
// Bei 45° bleibt das Rad rechts stehen und nur das linke dreht noch. Der Roboter dreht um das rechte Rad.
// Bei 90° dreht das rechte Rad rückwärts. Der Roboter dreht sich um die eigene Achse.
// Bei 135° dreht sich der Roboter um das linke Rad, aber jetzt rückwärts.
// Bei 180° fährt das Fz gerade rückwärts.
// Dasselbe passiert spiegelverkehrt auf die andere Seite (Winkel < 0) bis -180 Grad.
// speed ist die Geschwindigkeit und reicht von 0 - 100
S_vector robokit_make_vector_polar(int16_t angle, T_Speed speed);

// Prüfe, ob zwei Vektoren gleich sind.
uint8_t robokit_vector_equals(S_vector vec1, S_vector vec2);

#endif /* vector_h */
