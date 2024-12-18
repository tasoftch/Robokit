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
//  test_unit.c
//  test_bench
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#include "test_unit.h"
#include "_test_unit.h"
#include <stdarg.h>

static int tests_done=0;
static int tests_failed=0;
static int tests_risky=0;

static int ts_count = 0;
static int ts_failed = 0;

static int tc_count = 0;
static int tc_failed = 0;

static void (*_test_suite_functions[TA_MAX_TESTS])(void);
static const char *_test_suite_names[TA_MAX_TESTS];
static int _test_suites_count = 0;

void __ta_register_test(void (*func)(void), const char *name) {
    if (_test_suites_count < TA_MAX_TESTS) {
        _test_suite_functions[_test_suites_count] = func;
        _test_suite_names[_test_suites_count] = name;
        _test_suites_count++;
    } else {
        printf("Maximale Testanzahl erreicht!\n");
    }
}

static void _test_suite_init(void) {
    ts_count = 0;
    ts_failed = 0;
    tc_count = 0;
    tc_failed = 0;
}

void _tu_print_error(const char *fn_name, int line, int num_args, ...) {
    va_list args;
    va_start(args, num_args);

    printf("        ** %s:%d ", fn_name, line);
    
    for (int i = 0; i < num_args; i++) {
        const char *arg = va_arg(args, const char *);
        if (arg != NULL) {
            printf("%s", arg);
        }
    }
    
    printf(".\n");

    va_end(args);
}

void _test_inc(void) {
    tests_done++;
    tc_count++;
    ts_count++;
}
void _test_failed(void) {
    tests_failed++;
    tc_failed++;
    ts_failed++;
}

void TARunTests(void) {
    for(int i = 0; i < _test_suites_count; i++) {
        printf("== Start Test Suite %s ==\n", _test_suite_names[i]);
        _test_suite_init();
        _test_suite_functions[i]();

        if(tc_count < 1) {
            printf("    :: Risky! No assertion performed.\n\n");
            tests_risky++;
            continue;
        }

        if(tc_failed < 1) {
            printf("    :: Passed with %d assertions.\n\n", tc_count);
        } else {
            printf("    :: Failed with %d failed of %d assertions.\n\n", tc_failed, tc_count);
        }
    }

    if(tests_done < 1) {
        printf(":: Risky! No assertion performed.\n\n");
        return;
    }

    printf("************** CONCLUSION **************\n");

    if(tests_failed < 1) {
        if(tests_risky) {
            printf("*  Passed with %d assertions. ==\n", tests_done);
            printf("** BUT CONTAINS %d RISKY TESTS. ==\n", tests_risky);
        } else {
            printf("*  Passed with %d assertions. ==\n", tests_done);
        }
    } else {
        printf("** Failed with %d failed of %d assertions.==\n", tests_done, tests_failed);
        printf("*  Passed %d assertions. ==\n", tests_done);
    }
    printf("****************************************\n\n");
}