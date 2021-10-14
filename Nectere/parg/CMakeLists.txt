include_directories(..)

set(PARG_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/parg.h
	${CMAKE_CURRENT_SOURCE_DIR}/parg.c
	${CMAKE_CURRENT_SOURCE_DIR}/parg_def.h)
	
set_source_files_properties(${PARG_SOURCES} PROPERTIES LANGUAGE CXX)

add_library(${NAME}-parg ${PARG_SOURCES})
target_include_directories(${NAME}-parg PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})