
file(GLOB_RECURSE MY_FILES "src/*.c")

idf_component_register(SRCS ${MY_FILES}
        INCLUDE_DIRS "./include"
        REQUIRES driver espressif__led_strip esp_timer esp_partition esp_wifi
)