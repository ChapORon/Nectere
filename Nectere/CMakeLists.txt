cmake_minimum_required(VERSION 3.19)

set(NAME Nectere)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.75.0 REQUIRED system regex)

if (Boost_FOUND)
    include_directories(.)
    include_directories(${Boost_INCLUDE_DIRS}) 
    link_directories(${Boost_LIBRARY_DIRS})

    set(LIB_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ACommand.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ACommand.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ISessionHandler.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Server.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Server.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Session.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Session.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/UIDVector.hpp)

    set(SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/Command/ANectereCommand.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Command/ANectereCommand.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Command/StopCommand.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Command/StopCommand.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/DynamicLibrary.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
            ${LIB_FILES})
            
    add_executable(${NAME}-server ${SOURCE_FILES})
    add_library(${NAME} SHARED ${LIB_FILES})
    add_library(${NAME}-s STATIC ${LIB_FILES})
    target_link_libraries(${NAME} ${Boost_LIBRARIES})
    add_custom_command(TARGET ${NAME}-server POST_BUILD COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_SOURCE_DIR}/out/build/x64-Debug/Nectere/plugins)
    add_custom_command(TARGET ${NAME}-server POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/plugins ${PROJECT_SOURCE_DIR}/out/build/x64-Debug/Nectere/plugins)
    message(${PROJECT_SOURCE_DIR}/out/build/x64-Debug/Nectere)
endif()
