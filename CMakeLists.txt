# For more information about build system see
# https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/build-system.html
# The following five lines of boilerplate have to be in your project's
# CMakeLists in this exact order for cmake to work correctly
cmake_minimum_required(VERSION 3.16)

if ( "$ENV{__TEST__}" STREQUAL "1")
    project(Robo_Test LANGUAGES C CXX ASM)
    file(GLOB_RECURSE all_SRC
            "components/hw_drive/Tests/*.c"
        "components/hw_drive/src/vector.c"
        "components/hw_drive/src/utils/motor_logic.c"
            "components/hw_drive/src/utils/median.c"
    )
    set(TARGET Robo_Test.bin)
    add_executable(${TARGET} ${all_SRC})
    file(GLOB_RECURSE HEADER_FILES "components/hw_drive/*.h")

    set(HEADER_DIRS "")
    foreach(HEADER_FILE ${HEADER_FILES})
        get_filename_component(HEADER_DIR ${HEADER_FILE} PATH)
        list(APPEND HEADER_DIRS ${HEADER_DIR})
    endforeach()

    list(REMOVE_DUPLICATES HEADER_DIRS)

    target_include_directories(${TARGET} PRIVATE ${HEADER_DIRS})
    target_compile_definitions(${TARGET} PRIVATE __TEST__)
else()
    set(ESP_PLATFORM "esp32c6")
    set(ENV{IDF_TARGET} "esp32c6")
    include($ENV{IDF_PATH}/tools/cmake/project.cmake)
    project(Robo)
    # Damit die automatische Registrierung der Module in hw_drive funktioniert.
    target_link_options(Robo.elf PRIVATE "-Wl,--whole-archive" "${CMAKE_CURRENT_BINARY_DIR}/esp-idf/hw_drive/libhw_drive.a" "-Wl,--no-whole-archive")
endif ()