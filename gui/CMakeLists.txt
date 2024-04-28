set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_SOURCE_FILES	pinc_gui.cpp
							pincMainWindow.cpp
							pincControlModeButtons.cpp
							pincStepperControl.cpp
							pincJogControl.cpp)

set(LINKED_LIBRARIES		Qt5::Core
							Qt5::Widgets
							c_pinc)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})							

add_library				(guiLib	${LIBRARY_SOURCE_FILES}	)
target_link_libraries	(guiLib	${LINKED_LIBRARIES}		)
target_compile_options	(guiLib	PRIVATE	"-Wno-psabi"	)