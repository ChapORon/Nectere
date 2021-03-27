cmake_minimum_required (VERSION 3.8)

set(NAME Nectere)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.75.0 REQUIRED system regex)

if(Boost_FOUND)
    include_directories(.)
    include_directories(${PROJECT_SOURCE_DIR}/DataParser)
    include_directories(${Boost_INCLUDE_DIRS}) 
    link_directories(${Boost_LIBRARY_DIRS})

    set(SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/Command/ANectereCommand.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Command/ANectereCommand.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ACommand.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ACommand.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/ISessionHandler.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Server.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Server.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Session.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Session.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/UIDVector.hpp "Command/ANectereCommand.hpp" "Command/StopCommand.hpp" "Command/StopCommand.cpp")

    add_executable (${NAME} ${SOURCE_FILES})
    if (WIN32)
	    target_link_libraries(${NAME} ${Boost_LIBRARIES} DataParser-s)
    else()
	    target_link_libraries(${NAME} ${Boost_LIBRARIES} DataParser)
    endif()
endif()
