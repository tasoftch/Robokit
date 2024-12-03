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
//  commands.c
//  robokit
//
//  Created by Thomas Abplanalp on 07.11.24.
//

#include "main_commands.h"
#include "robokit_log.h"

void _robokit_command_init(S_command *cmd) {
    if(cmd) {
        *cmd = ((S_command){0});
    }
}

void _commands_init(void) {
    ROBOKIT_LOGI("Commands init");
}


/*
uint8_t robokit_make_drive_command_fal_enable(S_command *cmd, uint8_t speed) {
    if(!cmd)
        return 0;
    
    _robokit_command_init(cmd);
    cmd->cmd = E_COMMAND_FAL_ENABLE;
    cmd->dta.uint8.a = speed;
    
    return 1;
}

uint8_t robokit_drive_command_set_pointer(S_command *cmd, void *ptr) {
    if(!cmd)
        return 0;
    
#if defined(__i386__) || defined(_M_IX86)
    cmd->ptr = ptr;
    return 1;
#else
    return 0;
#endif
}

void *robokit_drive_command_get_pointer(S_command *cmd) {
    if(!cmd)
        return 0;
    
#if defined(__i386__) || defined(_M_IX86)
    return cmd->ptr;
#else
    return NULL;
#endif
}


uint8_t robokit_make_drive_command_fal_disable(S_command *cmd);

// noch nicht ganz fertig.
uint8_t robokit_make_drive_command_fal_pattern(S_command *cmd);
*/