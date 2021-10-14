include_directories(..)

set(CONCURRENCY_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Task.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Task.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/TaskResult.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Thread.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Thread.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThreadSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThreadSystem.hpp)
	
add_library(${NAME}-concurrency ${CONCURRENCY_SOURCES})
target_include_directories(${NAME}-concurrency PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})