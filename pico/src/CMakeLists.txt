cmake_minimum_required(VERSION 3.12)

include(/home/mike/projects/pico/pico-sdk/external/pico_sdk_import.cmake)

pico_sdk_init()

project(i2c_buttons C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.3.0")
    message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.3.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")

endif()
if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
    add_compile_options(-Wno-maybe-uninitialized)
endif()

include_directories(${CMAKE_CURRENT_LIST_DIR}/../..)

add_subdirectory(oled)
add_subdirectory(spi)

add_executable              (pioSPI)
target_sources              (pioSPI PRIVATE stepper.c pinc.c)
target_link_libraries       (pioSPI spilib hardware_gpio hardware_dma hardware_pio pico_multicore hardware_i2c)
pico_add_extra_outputs      (pioSPI)
pico_enable_stdio_usb       (pioSPI 1)
pico_enable_stdio_uart      (pioSPI 0)