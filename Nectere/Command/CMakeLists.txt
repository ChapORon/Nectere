include_directories(..)

set(LIB_NAME ${NAME}Command)
set(NECTERE_DEPENDENCIES ${NAME}Dp)
set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 0)
set(LIB_VERSION_PATCH 0)
set(LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

set(COMMAND_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/ACanalCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ACanalCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ApplicationListCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ApplicationListCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/AuthCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AuthCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/CommandListCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CommandListCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/CreateCanalCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CreateCanalCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeleteCanalCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/DeleteCanalCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/JoinCanalCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/JoinCanalCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/LeaveCanalCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/LeaveCanalCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ListCanalCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ListCanalCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/MessageCanalCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/MessageCanalCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/PingCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/PingCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ReloadAppCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ReloadAppCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/StopCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StopCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/UnloadAppCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/UnloadAppCommand.hpp)
	
add_library(${LIB_NAME} ${COMMAND_SOURCES})
target_link_libraries(${LIB_NAME} ${NECTERE_DEPENDENCIES})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})