
#include <device.h>
#include <imu_commands.h>
#include <private/robokit_log.h>

#include "wlan_ap.h"
#include "socket_server.h"

static uint8_t my_interpreter(uint8_t *buffer, uint8_t length) {
    uint8_t cmd = buffer[0];
    if(cmd < ROBOKIT_MAX_SCHEDULED_COMMANDS) {
        if(cmd == E_COMMAND_VECTOR) {
            int8_t angle = buffer[1];
            uint8_t speed = buffer[2];
            S_vector vec = {angle, speed};
            S_command command;

            if(speed < 1) {
                robokit_make_drive_command_fwd(&command, 0);
                robokit_push_command(&command, 0);
                robokit_make_command_imu_stop(&command);
                robokit_push_command(&command, 0);
            } else {
                robokit_make_drive_command_vector(&command, vec);
                robokit_push_command(&command, 0);
            }

            return 0;
        }
        if(length >= 8) {
            S_command *command = (S_command *)buffer;
            robokit_push_command(command, 0);
        } else {
            ROBOKIT_LOGE("Invalid command %02X with length %d", cmd, length);
        }
        return 0;
    } else {
        ROBOKIT_LOGI("Status command %02X", cmd);
        switch (cmd) {
            case 0xF3:
                S_motor_config lcfg = robokit_motor_left_get_config();
                S_motor_config rcfg = robokit_motor_right_get_config();
                buffer[0] = 0xF3;
                buffer[1] = *(uint8_t*) &lcfg;
                buffer[2] = *(uint8_t*) &rcfg;
                buffer[3] = 0x0;
                buffer[4] = 0x0;
                buffer[5] = 0x0;
                buffer[6] = 0x0;
                buffer[7] = 0x0;
            return 8;
            default:
        }
    }
    return 0xFF;
}

void app_main()
{
    device_init();
    S_command cmd;
    robokit_make_test_command(&cmd);
    robokit_push_command(&cmd, 0);

    wifi_init_softap();
    socket_server_init();
    socket_server_interpreter(my_interpreter);

    robokit_make_test_command(&cmd);
    robokit_push_command(&cmd, 0);
}
