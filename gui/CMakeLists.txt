set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_SOURCE_FILES	pinc_gui.cpp
							pinc_buttonGroup.cpp)

set(LINKED_LIBRARIES		Qt5::Core
							Qt5::Widgets)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})							

add_library				(guiLib	${LIBRARY_SOURCE_FILES}	)
target_link_libraries	(guiLib	${LINKED_LIBRARIES}		)
target_compile_options	(guiLib	PRIVATE	"-Wno-psabi"	)

add_executable			(gui_test gui_test.cpp)
target_link_libraries	(gui_test guiLib)
target_compile_options	(gui_test PRIVATE "-Wno-psabi")