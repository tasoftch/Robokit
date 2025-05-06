
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

	vTaskDelay(200 / portTICK_PERIOD_MS);

    /*wifi_init_softap();
    socket_server_init();
    socket_server_interpreter(tcp_command_interpreter);
	*/
	robokit_system_ready_indication();

	S_command cmd;
	robokit_make_drive_command_fwd(&cmd, 30);
	robokit_push_command(&cmd, 0);

	robokit_make_command_us_stop_at_distance(&cmd, 20);
	robokit_push_command(&cmd, 0);

	vTaskDelay(10000 / portTICK_PERIOD_MS);
	robokit_make_drive_command_fwd(&cmd, 0);
	robokit_push_command(&cmd, 0);
}
