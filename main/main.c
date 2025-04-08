
#include <device.h>
#include "wlan_ap.h"
#include "socket_server.h"
#include "tcp_command_interpreter.h"



void app_main()
{
    device_init();
    S_command cmd;
    robokit_make_test_command(&cmd);
    robokit_push_command(&cmd, 0);

    wifi_init_softap();
    socket_server_init();
    socket_server_interpreter(tcp_command_interpreter);

    robokit_make_test_command(&cmd);
    robokit_push_command(&cmd, 0);
}
