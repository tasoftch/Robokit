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

#include <hal/fal.h>
#include <vector.h>
#include <device.h>
#include <private/robokit_log.h>
#include "fal_commands.h"

static int8_t static_commands_map[] = {
	0, 		    // -----
	-45, 		// ----S
	-30, 		// ---S-
	-45, 		// ---SS
	0, 		    // --S--
	-30, 		// --S-S
	-12, 		// --SS-
	-30, 		// --SSS
	30, 		// -S---
	-3, 		// -S--S
	0, 		    // -S-S-
	-3, 		// -S-SS
	12, 		    // -SS--
	12, 		    // -SS-S
	0, 		    // -SSS-
	-3, 		// -SSSS
	45, 		// S----
	45, 		    // S---S
	30, 		    // S--S-
	-3, 		// S--SS
	12, 		    // S-S--
	0, 		    // S-S-S
	0, 		    // S-SS-
	0, 		    // S-SSS
	30, 		// SS---
	30, 		// SS--S
	30, 		    // SS-S-
	0, 		    // SS-SS
	30, 		// SSS--
	30, 		// SSS-S
	0, 		    // SSSS-
	0 		    // SSSSS
};

static int8_t static_commands_acceptance_map[] = {
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // -----
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// ----S
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// ---S-
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// ---SS
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 		    // --S--
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// --S-S
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// --SS-
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// --SSS
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// -S---
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 			// -S--S
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // -S-S-
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 			// -S-SS
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 		    // -SS--
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // -SS-S
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 		    // -SSS-
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// -SSSS
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// S----
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // S---S
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // S--S-
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 			// S--SS
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // S-S--
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // S-S-S
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // S-SS-
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // S-SSS
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// SS---
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 			// SS--S
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // SS-S-
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 		    // SS-SS
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 			// SSS--
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED, 			// SSS-S
	ROBOKIT_FAL_INTERPRETER_RESULT_ACCEPTED, 		    // SSSS-
	ROBOKIT_FAL_INTERPRETER_RESULT_DECLINED 		    // SSSSS
};

/**
 * @interitDoc
 */
robokit_fal_interpreter_result_t fal_default_line_result_interpreter(S_Fal_Result *result, T_Speed speed) {
	S_command cmd;
	S_vector vector;

	uint8_t flags = 0;

	flags |= (result->fb_1_left == ROBOKIT_FAL_BLACK) ? (1<<4) : 0;
	flags |= (result->fb_2_middle_left == ROBOKIT_FAL_BLACK) ? (1<<3) : 0;
	flags |= (result->fb_3_middle == ROBOKIT_FAL_BLACK) ? (1<<2) : 0;
	flags |= (result->fb_4_middle_right == ROBOKIT_FAL_BLACK) ? (1<<1) : 0;
	flags |= (result->fb_5_right == ROBOKIT_FAL_BLACK) ? (1<<0) : 0;

	vector = robokit_make_vector_polar(static_commands_map[flags], speed);

	robokit_make_drive_command_vector(&cmd, vector);
	robokit_push_command(&cmd, 0);

	return static_commands_acceptance_map[flags];
}