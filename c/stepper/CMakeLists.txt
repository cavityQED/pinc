include_directories(${CMAKE_SOURCE_DIR})

add_executable			(stepper_test	stepper_test.c pi_stepper.c)
target_link_libraries	(stepper_test	pigpio pthread c_pinc)