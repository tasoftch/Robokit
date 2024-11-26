idf_component_register(SRCS "main.c"
                            "connect_wifi.c"
                            "driver.c"
                    INCLUDE_DIRS ".")
                    

                    spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
