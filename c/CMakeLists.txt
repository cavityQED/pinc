set(CMAKE_C_STANDARD 11)
project(c_pinc C)

include_directories(${CMAKE_SOURCE_DIR}/c)

set(LIBRARY_SOURCE_FILES	stepper/pi_stepper.c
							algorithm/bst.c)

set(LINKED_LIBRARIES		pigpio
							pthread
							m)

add_subdirectory(spi)
add_subdirectory(io)
add_subdirectory(stepper)						

add_library					(c_pinc	${LIBRARY_SOURCE_FILES}			)
target_link_libraries		(c_pinc	${LINKED_LIBRARIES}				)
target_compile_options		(c_pinc	PRIVATE	"-Wno-psabi"			)
target_include_directories	(c_pinc INTERFACE ${CMAKE_SOURCE_DIR}	)