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

#include <stdio.h>
#include "test_unit.h"
#include "vector.h"
#include "motor_logic.h"
#include "private/median.h"

TEST_SUITE("Vectors Test") {
    S_vector vector = robokit_make_vector(0, 100);

    TAAssertVectorHasSpeed(vector, 100);
    TAAssertVectorHasAngle(vector, 0);

    vector = robokit_make_vector(50, 50);

    TAAssertVectorHasSpeed(vector, 70);
    TAAssertVectorHasAngle(vector, 45);
    TAAssertTrue(robokit_vector_equals(vector, ((S_vector){45 / 3, 70})));

    // nur nach rechts
    vector = robokit_make_vector(80, 0);

    TAAssertVectorHasSpeed(vector, 80);
    TAAssertVectorHasAngle(vector, 90);

    // nach rechts hinten 45°
    vector = robokit_make_vector(50, -50);

    TAAssertVectorHasSpeed(vector, 70);
    TAAssertVectorHasAngle(vector, 135);

    // nur nach hinten
    vector = robokit_make_vector(0, -80);

    TAAssertVectorHasSpeed(vector, 80);
    TAAssertVectorHasAngle(vector, 180);

    // nach links oben

    vector = robokit_make_vector(-50, 50);

    TAAssertVectorHasSpeed(vector, 70);
    TAAssertVectorHasAngle(vector, -45);

    // nur nach links
    vector = robokit_make_vector(-80, 0);

    TAAssertVectorHasSpeed(vector, 80);
    TAAssertVectorHasAngle(vector, -90);


    // Nach links hinten
    vector = robokit_make_vector(-50, -50);

    TAAssertVectorHasSpeed(vector, 70);
    TAAssertVectorHasAngle(vector, -135);

    // Ungültiger Vektor, welcher eine Geschwindigkeit von mehr als 100 hat
    vector = robokit_make_vector(80, 80);

    TAAssertTrue(robokit_vector_equals(vector, ROBOKIT_INVALID_VECTOR));

    // nur nach hinten
    vector = robokit_make_vector(-1, -80);

    TAAssertVectorHasSpeed(vector, 80);
    TAAssertVectorHasAngle(vector, -180);
}

TEST_SUITE("Motor Controller") {
    // Motor Stop
    S_vector vector = robokit_make_vector_polar(0, 0);
    S_motor_ctl motor_left={0}, motor_right={0};

    robokit_vector_to_motor(vector, &motor_left, &motor_right);

    TAAssertMotorForward(motor_left, 0);
    TAAssertMotorForward(motor_right, 0);

    // Volle Kraft voraus!
    vector = robokit_make_vector_polar(0, ROBOKIT_MAX_SPEED);
    robokit_vector_to_motor(vector, &motor_left, &motor_right);

    TAAssertMotorForward(motor_left, 100);
    TAAssertMotorForward(motor_right, 100);

    // Volle Kraft nach rechts!
    vector = robokit_make_vector_polar(45, ROBOKIT_MAX_SPEED);
    robokit_vector_to_motor(vector, &motor_left, &motor_right);

    TAAssertMotorForward(motor_left, 100);
    TAAssertMotorForward(motor_right, 0);

    // Volle Kraft nach links!
    vector = robokit_make_vector_polar(90, ROBOKIT_MAX_SPEED);
    robokit_vector_to_motor(vector, &motor_left, &motor_right);

    TAAssertMotorForward(motor_left, 100);
    TAAssertMotorBackward(motor_right, 100);

    // Halbe Geschwindigkeit nach 2 Uhr
    vector = robokit_make_vector_polar(30, 50);
    robokit_vector_to_motor(vector, &motor_left, &motor_right);

    TAAssertMotorForward(motor_left, 50);
    TAAssertMotorForward(motor_right, 25);
}

TEST_SUITE("Median tests") {
	S_robokit_median_filter_t median_filter = {0};
	robokit_median_filter_init(&median_filter);

	TAAssertEqual(0, median_filter.index);
	TAAssertEqual(0, median_filter.count);
	TAAssertEqual(0, median_filter.buffer[0]);

	robokit_median_filter_add(&median_filter, 5);
	TAAssertEqual(5, robokit_median_filter_get(&median_filter));

	robokit_median_filter_add(&median_filter, 2);
	TAAssertEqual(5, robokit_median_filter_get(&median_filter));

	robokit_median_filter_add(&median_filter, 8);
	TAAssertEqual(5, robokit_median_filter_get(&median_filter));

	robokit_median_filter_add(&median_filter, 3);
	TAAssertEqual(5, robokit_median_filter_get(&median_filter));

	robokit_median_filter_add(&median_filter, 6);
	TAAssertEqual(5, robokit_median_filter_get(&median_filter));

	robokit_median_filter_add(&median_filter, 20);
	TAAssertEqual(6, robokit_median_filter_get(&median_filter));
}

int main() {
	TARunTests();
	return 0;
}


