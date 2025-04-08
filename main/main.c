
#include <device.h>
#include <imu_commands.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <private/robokit_log.h>
#include <portmacro.h>

void app_main()
{
    device_init();

    vTaskDelay(1000 / portTICK_PERIOD_MS);

    S_command cmd;

    robokit_make_command_imu_drive_forward(&cmd, 50);
    robokit_push_command(&cmd, 0);

    vTaskDelay(20000 / portTICK_PERIOD_MS);

    robokit_make_command_imu_stop(&cmd);
    robokit_push_command(&cmd, 0);
}
