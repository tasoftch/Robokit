#if !defined(DRIVER_H_)
#define DRIVER_H_

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

void createrampcmdf(uint8_t);

void createrampcmdb(uint8_t);

void do_45(void);

void do_90(void);

#endif // DRIVER_H_
