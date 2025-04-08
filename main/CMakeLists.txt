idf_component_register(SRCS "main.c"
                            "wlan_ap.c"
                            "socket_server.c"
        "tcp_command_interpreter.c"
        ".")
                    

                   # spiffs_create_partition_image(storage ../data FLASH_IN_PROJECT)
