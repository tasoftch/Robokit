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
//  _test_unit.h
//  test_bench
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#ifndef _test_unit_h
#define _test_unit_h

#include <stdio.h>

#define PRINT_ARGS(...) _tu_print_error(__FILE__, __LINE__, sizeof((const char*[]){__VA_ARGS__}) / sizeof(const char*), __VA_ARGS__)
void _tu_print_error(const char *fn_name, int line, int num_args, ...);

void _test_inc(void);
void _test_failed(void);

void __tu_tests_init(const char *name, const char *fn_names, ...);

#define _tu_init_test_suite(NAME, ...) \
({\
    __tu_tests_init(NAME, #__VA_ARGS__, __VA_ARGS__, NULL);\
})


#define _tu_assert_true(EXP, EXP_STR) \
({\
    _test_inc();\
    if(!(EXP)) {\
        PRINT_ARGS(": Failed asserting that ", EXP_STR, " is true"); \
        _test_failed();\
    } \
})

#define _tu_assert_false(EXP, EXP_STR) \
({\
    _test_inc();\
    if((EXP)) {\
        PRINT_ARGS(": Failed asserting that ", EXP_STR, " is false"); \
        _test_failed();\
    } \
})

#define _tu_assert_equals(EXP1, EXP1_STR, EXP2, EXP2_STR) \
({\
    _test_inc();\
    __typeof__(EXP1) EXP_VAL1 = (EXP1); \
    __typeof__(EXP2) EXP_VAL2 = (EXP2); \
    if(EXP_VAL1 != EXP_VAL2) {\
        PRINT_ARGS(": Failed asserting that ", EXP1_STR, " is equal to ", EXP2_STR); \
        _test_failed();\
    }\
})

#define _tu_assert_nequals(EXP1, EXP1_STR, EXP2, EXP2_STR) \
({\
    _test_inc();\
    __typeof__(EXP1) EXP_VAL1 = (EXP1); \
    __typeof__(EXP2) EXP_VAL2 = (EXP2); \
    if(EXP_VAL1 == EXP_VAL2) {\
        PRINT_ARGS(": Failed asserting that ", EXP1_STR, " is not equal to ", EXP2_STR); \
        _test_failed();\
    }\
})

#define _tu_assert_vequals(EXP1, EXP1_STR, EXP2, EXP2_STR) \
({\
    _test_inc();\
    __typeof__(EXP1) EXP_VAL1 = (EXP1); \
    __typeof__(EXP2) EXP_VAL2 = (EXP2); \
    if(!robokit_vector_equals(EXP_VAL1, EXP_VAL2)) {\
        printf("        %s:%d : Failed asserting that vector {%d°, %d} is equal to %s\n",__FILE__, __LINE__, EXP_VAL1.angle, EXP_VAL1.speed, EXP2_STR);\
        _test_failed();\
    }\
})

#define _tu_assert_vsequals(VEC1, VEC1_STR, SPEED, SPEED_STR) \
({\
    _test_inc();\
    __typeof__(VEC1) EXP_VAL1 = (VEC1); \
    if(EXP_VAL1.speed != SPEED) {\
        printf("        %s:%d : Failed asserting that vector {%d°, %d} has a speed of %s\n", __FILE__, __LINE__, EXP_VAL1.angle, EXP_VAL1.speed, SPEED_STR);\
        _test_failed();\
    }\
})

#define _tu_assert_vaequals(VEC1, VEC1_STR, ANGLE, ANGLE_STR) \
({\
    _test_inc();\
    __typeof__(VEC1) EXP_VAL1 = (VEC1); \
    if(EXP_VAL1.angle*3 != ANGLE) {\
        printf("        %s:%d : Failed asserting that vector {%d°, %d} has an angle of %s°\n",__FILE__, __LINE__, EXP_VAL1.angle, EXP_VAL1.speed, ANGLE_STR);\
        _test_failed();\
    }\
})


#define _tu_assert_motor(MT_CTL, MT_CTL_STR, SPEED, SPEED_STR) \
({\
    _test_inc();\
    S_motor_ctl EXP_VAL1 = (MT_CTL); \
    if(EXP_VAL1.direction != (uint8_t)1 || EXP_VAL1.speed != (uint8_t)SPEED) {\
        PRINT_ARGS(": Failed asserting that motor ", MT_CTL_STR, " drives forward with speed ", SPEED_STR); \
        _test_failed();\
    }\
})

#endif /* _test_unit_h */
