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

//
//  test_unit.h
//  test_bench
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#ifndef test_unit_h
#define test_unit_h

#define TA_MAX_TESTS 20

#include "_test_unit.h"

#define TEST_SUITE( CAPTION, ... ) _TEST_INTERNAL_CREATE( _TA_TEST_MAKE_NAME(_TA_INTERNAL_TEST, __LINE__), CAPTION )

void TARunTests(void);

#define TAAssertTrue(EXPRESSION, ...) \
_tu_assert_true(EXPRESSION, #EXPRESSION)

#define TAAssertFalse(EXPRESSION, ...) \
_tu_assert_false(EXPRESSION, #EXPRESSION)

#define TAAssertEqual(EXP1, EXP2, ...) \
_tu_assert_equals(EXP1, #EXP1, EXP2, #EXP2)

#define TAAssertNotEqual(EXP1, EXP2, ...) \
_tu_assert_nequals(EXP1, #EXP1, EXP2, #EXP2)

#define TAAssertEqualVectors(VEC1, VEC2, ...) \
_tu_assert_vequals(VEC1, #VEC1, VEC2, #VEC2)

#define TAAssertVectorHasSpeed(VEC1, SPEED, ...) \
_tu_assert_vsequals(VEC1, #VEC1, SPEED, #SPEED)

#define TAAssertVectorHasAngle(VEC1, ANGLE, ...) \
_tu_assert_vaequals(VEC1, #VEC1, ANGLE, #ANGLE)

#define TAAssertMotorForward(MT_CTL, SPEED, ...) \
_tu_assert_motor(MT_CTL, #MT_CTL, SPEED, #SPEED)

#endif /* test_unit_h */
