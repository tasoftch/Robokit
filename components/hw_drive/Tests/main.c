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

#include <robokit_module.h>
#include <stdio.h>
#include <timed_commands.h>

#include "test_unit.h"
#include "vector.h"
#include "motor_logic.h"
#include "timed_commands_impl.h"


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

TEST_SUITE("Timed Commands") {
	// Reduce ROBOKIT_TIMED_COMMAND_LISTS_ITEMS_STACK_COUNT to 5
	TAAssertEqual(5, tc_command_get_available());
	S_T_cmd *cmd = tc_command_alloc();

	TAAssertEqual(4, tc_command_get_available());
	TAAssertNotEqual(NULL, cmd);

	S_T_cmd *cmd1 = tc_command_alloc();
	S_T_cmd *cmd2 = tc_command_alloc();
	S_T_cmd *cmd3 = tc_command_alloc();

	TAAssertNotEqual(cmd1, cmd3);

	TAAssertEqual(1, tc_command_get_available());

	TAAssertEqual(2, cmd2 - cmd);

	tc_command_free(NULL);
	tc_command_free(cmd);
	tc_command_free(cmd1);

	TAAssertEqual(3, tc_command_get_available());
	S_T_cmd *cmd4 = tc_command_alloc();
	TAAssertNotEqual(NULL, cmd);

	S_T_cmd *cmd5 = tc_command_alloc();
	TAAssertNotEqual(NULL, cmd);

	S_T_cmd *cmd6 = tc_command_alloc();
	TAAssertNotEqual(NULL, cmd);

	TAAssertEqual(0, tc_command_get_available());

	cmd = tc_command_alloc();
	TAAssertEqual(NULL, cmd);

	tc_command_free(cmd);
	tc_command_free(cmd1);
	tc_command_free(cmd2);
	tc_command_free(cmd3);
	tc_command_free(cmd4);
	tc_command_free(cmd5);
	tc_command_free(cmd6);

	TAAssertEqual(5, tc_command_get_available());



	// Test chain
	S_T_chain *chain = tc_chain_alloc();
	tc_chain_init(chain);

	S_command command = ROBOKIT_COMMAND_INIT;

	TAAssertNotEqual(NULL, chain);
	command.cmd = 15;
	command.data[2] = 8;
	tc_chain_push_command(chain, 100, &command, 0);

	command = ROBOKIT_COMMAND_INIT;
	command.cmd = 5;
	command.data[1] = 3;
	tc_chain_push_command(chain, -100, &command, 0);

	command = ROBOKIT_COMMAND_INIT;
	command.cmd = 99;
	command.data[5] = 9;
	tc_chain_push_command(chain, 50, &command, 0);

	TAAssertEqual(3, chain->length);
	TAAssertEqual(250, chain->duration_ms);

	TAAssertEqual(0, tc_runtime_free_buffer_count());
}

int main() {
	TARunTests();
	return 0;
}


