add_library(oled INTERFACE)
target_sources(oled INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/oled.c)
target_link_libraries(oled INTERFACE pico_stdlib hardware_i2c)
target_include_directories(oled INTERFACE ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})