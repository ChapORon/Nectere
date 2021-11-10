include_directories(..)

set(DP_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Format.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Json.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Json.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Node.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Node.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Result.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Result.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Serializer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Serializer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Xml.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Xml.hpp)
	
add_library(${NAME}-dp ${DP_SOURCES})
target_include_directories(${NAME}-dp PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})