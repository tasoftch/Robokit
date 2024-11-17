//
//  commands.c
//  robokit
//
//  Created by Thomas Abplanalp on 07.11.24.
//

#include "main_commands.h"


void _robokit_command_init(S_command *cmd) {
    if(cmd) {
        *cmd = ((S_command){0});
    }
}

void _commands_init(void) {

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