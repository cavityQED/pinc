cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_COMPILER /usr/bin/g++)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 11)
project(pinc)

set(CMAKE_AUTOMOC ON)

add_subdirectory(c)

find_package(Qt5 COMPONENTS Core Widgets)
if(Qt5Core_FOUND)
	message("FOUND Qt5")
	message(STATUS "${Qt5Widgets_INCLUDE_DIRS}")

	add_subdirectory(device)
	add_subdirectory(spi)
	add_subdirectory(panel)
	add_subdirectory(gcode)

	include_directories(${CMAKE_SOURCE_DIR})

	set(libdocs		pincMainWindow.cpp
					pincLaser2D.cpp)

	set(LINKED_LIBRARIES	Qt5::Core
							Qt5::Widgets
							rt
							deviceLib
							pthread
							pigpio
							spiLib
							panelLib
							gcodeLib)

	add_library				(pincLib ${libdocs})
	target_link_libraries	(pincLib ${LINKED_LIBRARIES})
	target_compile_options	(pincLib PRIVATE "-Wno-psabi")

	add_executable			(pinc pinc.cpp)
	target_link_libraries	(pinc pincLib)
	target_compile_options	(pinc PRIVATE "-Wno-psabi")
endif()
