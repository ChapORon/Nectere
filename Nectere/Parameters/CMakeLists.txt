include_directories(..)

set(PARAMETERS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/LogInFileParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/LogInFileParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/LogPathParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/LogPathParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/NetworkPortParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/NetworkPortParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ScriptEngineSmartBuildParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ScriptEngineSmartBuildParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/VerboseParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VerboseParameter.hpp)
	
add_library(${NAME}-parameters ${PARAMETERS_SOURCES})
target_include_directories(${NAME}-parameters PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})