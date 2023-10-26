set(CMAKE_C_STANDARD 11)
project(c_pinc C)

include_directories(${CMAKE_SOURCE_DIR}/c)

set(LIBRARY_SOURCE_FILES	stepper/host_stepper.c
							lcd/st7735.c)

set(LINKED_LIBRARIES		pigpio
							pthread)

add_library					(c_pinc	${LIBRARY_SOURCE_FILES}			)
target_link_libraries		(c_pinc	${LINKED_LIBRARIES}				)
target_compile_options		(c_pinc	PRIVATE	"-Wno-psabi"			)
target_include_directories	(c_pinc INTERFACE ${CMAKE_SOURCE_DIR}	)

add_executable			(c_pinc_test	host_main.c)
target_link_libraries	(c_pinc_test	pigpio pthread c_pinc)

add_executable			(c_lcd_test		lcd_test.c)
target_link_libraries	(c_lcd_test		pigpio pthread c_pinc)