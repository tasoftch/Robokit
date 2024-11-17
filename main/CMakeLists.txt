idf_component_register(SRCS "main.c"
        "connect_wifi.c"
        INCLUDE_DIRS "."
        REQUIRES esp_http_server esp_wifi hw_drive nvs_flash vfs spiffs driver
)

spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
