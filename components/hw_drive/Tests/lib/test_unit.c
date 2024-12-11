//
//  test_unit.c
//  test_bench
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#include "test_unit.h"
#include "_test_unit.h"
#include <stdarg.h>
#include <ctype.h>

static int tests_done=0;
static int tests_failed=0;

static int ts_count = 0;
static int ts_failed = 0;

static int tc_count = 0;
static int tc_failed = 0;

static void _test_case_init(void) {
    tc_count = 0;
    tc_failed = 0;
}

static void _test_suite_init(void) {
    ts_count = 0;
    ts_failed = 0;
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

void _find_next_case_name(const char *fn_string, char *name, int *index) {
    int idx = 0;
    char cc=0;
    
    while ((cc = fn_string[(*index)++])) {
        if(isalpha(cc) || isdigit(cc) || cc == '_') {
            name[idx++] = cc;
        } else if(idx > 0)
            break;
    }
    name[idx] = '\0';
}

void __tu_tests_init(const char *name, const char *fn_names, ...) {
    printf("== Start Test Suite %s ==\n", name);
    va_list args;
    char buffer[100] = {0};
    
    va_start(args, fn_names);

    void (*fn)(void) = NULL;
    int idx=0;
    
    _test_suite_init();
    
    while ((fn = va_arg(args, void *))) {
        _find_next_case_name(fn_names, buffer, &idx);
        _test_case_init();
        
        printf("    = Start Test Case %s =\n", buffer);
        fn();
        printf("    = Test Case %s =\n", buffer);
        
        if(tc_count < 1) {
            printf("    :: Risky! No assertion performed.\n\n");
            continue;
        }
        
        if(tc_failed < 1) {
            printf("    :: Passed with %d assertions.\n\n", tc_count);
        } else {
            printf("    :: Failed with %d failed of %d assertions.\n\n", tc_failed, tc_count);
        }
    }

    printf("== Test Suite %s ==\n", name);
    
    if(ts_count < 1) {
        printf(":: Risky! No assertion performed.\n\n");
        return;
    }
    
    if(tc_failed < 1) {
        printf(":: Passed with %d assertions.\n\n", ts_count);
    } else {
        printf(":: Failed with %d failed of %d assertions.\n\n", ts_failed, ts_count);
    }
    
    va_end(args);
}
