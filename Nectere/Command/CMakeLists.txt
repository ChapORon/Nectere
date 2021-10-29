include_directories(..)

set(COMMAND_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/ANectereCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ANectereCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ApplicationListCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ApplicationListCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/CommandListCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CommandListCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/NectereEventCode.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/StopCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StopCommand.hpp)
	
add_library(${NAME}-command ${COMMAND_SOURCES})
target_link_libraries(${NAME}-command ${NAME}-network ${NAME}-dp)
target_include_directories(${NAME}-command PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})