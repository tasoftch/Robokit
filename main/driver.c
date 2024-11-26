#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <esp_http_server.h>
#include "nvs_flash.h"
#include "esp_spiffs.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "rom/gpio.h"
#include <dirent.h>       // For opendir(), readdir(), closedir()
#include "esp_vfs.h"

#include "connect_wifi.h"
#include <device.h>

static const char *TAG = "rampe";
void createrampcmdf(uint8_t power)
{
    float smallvar;
    float out;
    S_command cmd;
    for (size_t i = 0; i < power; i++)
    {
        robokit_make_drive_command_fwd(&cmd, i);
        robokit_push_command(&cmd,0);
      
        vTaskDelay(20/ portTICK_PERIOD_MS);
        ESP_LOGI(TAG,"rampe:%i",i);
    }
    ESP_LOGI(TAG,"Exiting...cmd");
    

    
}
void createrampcmdb(uint8_t powe)
{
    float smallvar;
    float out;
    S_command cmd;
    for (size_t i = 0; i < powe; i++)
    {
        robokit_make_drive_command_bwd(&cmd, i);
        robokit_push_command(&cmd,0);

        vTaskDelay(20/ portTICK_PERIOD_MS);
        ESP_LOGI(TAG,"rampe:%i",i);
    }
    ESP_LOGI(TAG,"Exiting...cmd");
}

void do_90(void)
{
    S_command cmd;
    for (size_t i = 0; i < 90; i++)
    {
        uint8_t b;
        //b = 90 - i
      
        //robokit_make_vector_polar(1,i);
        //robokit_push_command(); //wheres the cmd?
    }
    ESP_LOGI(TAG,"Exiting...cmd");
}

