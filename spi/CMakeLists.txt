set(CMAKE_CXX_STANDARD 20)
set(LIBRARY_SOURCE_FILES	spiMsg.cpp
							spiHost.cpp)

set(LINKED_LIBRARIES		Qt5::Core
							Qt5::Widgets)

include_directories(${Qt5Widgets_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR})							

add_library				(spiLib	${LIBRARY_SOURCE_FILES}	)
target_link_libraries	(spiLib	${LINKED_LIBRARIES}		)
target_compile_options	(spiLib	PRIVATE	"-Wno-psabi"	)