include_directories(${CMAKE_SOURCE_DIR})

add_executable			(c_gyro_test	gyro.c)
target_link_libraries	(c_gyro_test	pthread pigpio)

add_executable			(c_fpga_test	fpga.c)
target_link_libraries	(c_fpga_test	pthread pigpio)