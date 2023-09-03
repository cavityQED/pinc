set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_SOURCE_FILES	device.cpp
							spiDevice.cpp
							espStepperMotor.cpp
							espStepperGroup.cpp
							laser.cpp)

set(LINKED_LIBRARIES		Qt5::Core
							Qt5::Widgets
							spiLib
							gcodeLib)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})							

add_library				(deviceLib	${LIBRARY_SOURCE_FILES}	)
target_link_libraries	(deviceLib	${LINKED_LIBRARIES}		)
target_compile_options	(deviceLib	PRIVATE	"-Wno-psabi"	)