
#include <device.h>
#include <portmacro.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <private/robokit_log.h>

#include "wlan_ap.h"
#include "socket_server.h"
#include "tcp_command_interpreter.h"
#include "hc_sr04.h"


void app_main()
{
    device_init();
    //hc_sr04_init();

    S_command cmd;
    robokit_make_test_command(&cmd);
    robokit_push_command(&cmd, 0);

    wifi_init_softap();
    socket_server_init();
    socket_server_interpreter(tcp_command_interpreter);

    robokit_make_test_command(&cmd);
    robokit_push_command(&cmd, 0);

    /*
    while (1) {
        float distance = hc_sr04_read_distance_cm();
        ROBOKIT_LOGI("distance: %f", distance);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
    */
}
