//
//  test_unit.h
//  test_bench
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#ifndef test_unit_h
#define test_unit_h

#include "_test_unit.h"

#define TA_NULL_TERMINATION

void _TATestSuite(const char *name, ... TA_NULL_TERMINATION);

#define TATestSuite(NAME, ...) \
_tu_init_test_suite(NAME, __VA_ARGS__)

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
