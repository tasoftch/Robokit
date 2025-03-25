/*
 * MIT License
 *
 * Copyright (c) 2025 Th. Abplanalp, F. Romer
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
// Created by Thomas Abplanalp on 14.01.25.
//

#ifndef _MODULES_H
#define _MODULES_H

#define _ROBOKIT_MAKE_NAME2(NAME, LINE) NAME##LINE
#define _ROBOKIT_MAKE_NAME(NAME, LINE) _ROBOKIT_MAKE_NAME2(NAME, LINE)

void __robokit_register_init_callback(void(*)(void));
void __robokit_register_loop_callback(void(*)(void));

#define ROBOKIT_REGISTER_COMMAND_HANDLER_EX( CMD_NR, CMD_TYPE, NAME )\
	void _ROBOKIT_MAKE_NAME(_robokit_mod_, NAME) (CMD_TYPE *cmd, uint8_t mode, uint8_t *flags) __attribute__((used));\
	__attribute__((constructor, used)) static void _ROBOKIT_MAKE_NAME(_robokit_mod_register_, NAME)(void) { \
		robokit_register_command_fn(CMD_NR, _ROBOKIT_MAKE_NAME((F_command_callback) _robokit_mod_, NAME));  \
	}    \
	void _ROBOKIT_MAKE_NAME(_robokit_mod_, NAME) (CMD_TYPE *cmd, uint8_t mode, uint8_t *flags)

#define ROBOKIT_MODULE_INIT_EX(NAME, ...)\
	static void _ROBOKIT_MAKE_NAME(_robokit_init_, __LINE__) (void);\
	__attribute__((constructor(101))) static void _ROBOKIT_MAKE_NAME(_robokit_init_register, __LINE__)(void) { \
		__robokit_register_init_callback(_ROBOKIT_MAKE_NAME(_robokit_init_, __LINE__)); \
	}    \
	static void _ROBOKIT_MAKE_NAME(_robokit_init_, __LINE__) (void)

#define ROBOKIT_MODULE_LOOP_EX(NAME, ...)\
	void _ROBOKIT_MAKE_NAME(_robokit_loop_, __LINE__) (void) __attribute__((used));\
	__attribute__((constructor, used)) static void _ROBOKIT_MAKE_NAME(_robokit_loop_register_, __LINE__)(void) { \
	__robokit_register_loop_callback(_ROBOKIT_MAKE_NAME(_robokit_loop_, __LINE__));  \
	}    \
	void _ROBOKIT_MAKE_NAME(_robokit_loop_, __LINE__) (void)
#endif //_MODULES_H
