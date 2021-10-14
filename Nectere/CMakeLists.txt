cmake_minimum_required(VERSION 3.19)

set(NAME Nectere)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1")

add_subdirectory(Concurrency)
add_subdirectory(Network)
add_subdirectory(Command)
add_subdirectory(Dp)
add_subdirectory(Parameters)
add_subdirectory(parg)
add_subdirectory(Script)

add_definitions(-DUSE_BOOST)

include_directories(.)

set(SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/AApplicationHandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AApplicationHandler.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ACommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ACommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ApplicationManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ApplicationManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/AUser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AUser.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Configuration.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Configuration.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/DynamicLibrary.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Ptr.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/StringUtils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StringUtils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/UIDSet.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/UserManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/UserManager.hpp)

add_library(${NAME}-lib ${SOURCE_FILES})

target_link_libraries(${NAME}-lib ${NAME}-dp ${NAME}-parameters ${NAME}-parg)

set(EXECUTABLE_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/NectereExecutable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/NectereExecutable.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_executable(${NAME} ${EXECUTABLE_SOURCES})
target_link_libraries(${NAME} ${NAME}-lib ${NAME}-command)

set(TEST_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)

add_executable(${NAME}-Test ${TEST_SOURCES})
target_link_libraries(${NAME}-Test ${NAME}-lib ${NAME}-command)