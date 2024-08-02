set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_SOURCE_FILES	
							gBlock.cpp
							pincMove.cpp)

set(LINKED_LIBRARIES		Qt5::Core
							Qt5::Widgets)

include_directories(${CMAKE_SOURCE_DIR})												

add_library				(gcodeLib		${LIBRARY_SOURCE_FILES}	)
target_link_libraries	(gcodeLib		${LINKED_LIBRARIES}		)
target_compile_options	(gcodeLib		PRIVATE	"-Wno-psabi"	)