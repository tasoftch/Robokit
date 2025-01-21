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

// If you are developer and about to design a new module, you can include the header
// #include <modules/robokit_module.h>
// This provides 3 macros for proper usage for you:
// ROBOKIT_REGISTER_COMMAND_HANDLER			Used to check and apply enqueued commands
//

#ifndef ROBOKIT_MODULE_H
#define ROBOKIT_MODULE_H

#include <device.h>
#include <private/robokit_log.h>
#include <private/command_data_containers.h>
#include <hal/scheduler.h>

#include <private/_modules.h>

/**
 *
 */
#define ROBOKIT_REGISTER_COMMAND_HANDLER( CMD_NR, CMD_TYPE )     ROBOKIT_REGISTER_COMMAND_HANDLER_EX( CMD_NR, CMD_TYPE, CMD_NR )

/**
 *
 */
#define ROBOKIT_MODULE_INIT(NAME)								ROBOKIT_MODULE_INIT_EX(NAME)

#define ROBOKIT_COMMAND_RESET(CMD) ( CMD = (S_command){0} )
#define ROBOKIT_COMMAND_RESET_P(CMD) if(CMD) { *CMD = (S_command){0}; }
#endif //ROBOKIT_MODULE_H
