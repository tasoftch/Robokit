
#include <device.h>
#include <portmacro.h>
#include <freertos/FreeRTOS.h>
#include <private/robokit_log.h>

#include "hal/hc_sr04.h"

#include "wlan_ap.h"
#include "socket_server.h"
#include "tcp_command_interpreter.h"


void app_main()
{
    device_init();
    robokit_start_system_indication();

    wifi_init_softap();
    socket_server_init();
    socket_server_interpreter(tcp_command_interpreter);

	robokit_system_ready_indication();
}
