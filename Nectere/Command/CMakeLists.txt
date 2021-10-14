include_directories(..)

set(COMMAND_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/ANectereCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ANectereCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/NectereEventCode.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/StopCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StopCommand.hpp)
	
add_library(${NAME}-command ${COMMAND_SOURCES})
target_link_libraries(${NAME}-command ${NAME}-network)
target_include_directories(${NAME}-command PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})