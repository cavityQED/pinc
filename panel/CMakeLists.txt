set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_SOURCE_FILES	controlPanel.cpp
							positionReadout.cpp
							loadMenu.cpp
							laserPanel.cpp)

set(LINKED_LIBRARIES		Qt5::Core
							Qt5::Widgets)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})							

add_library				(panelLib	${LIBRARY_SOURCE_FILES}	)
target_link_libraries	(panelLib	${LINKED_LIBRARIES}		)
target_compile_options	(panelLib	PRIVATE	"-Wno-psabi"	)