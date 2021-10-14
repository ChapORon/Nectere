cmake_minimum_required (VERSION 3.19)

set(NAME NectereClient)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.76.0 REQUIRED system regex)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS}) 
    link_directories(${Boost_LIBRARY_DIRS})

    set(SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/Connection.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Connection.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Defines.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

    add_executable (${NAME} ${SOURCE_FILES})
    target_link_libraries(${NAME} ${Boost_LIBRARIES})
endif()
