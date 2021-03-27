cmake_minimum_required (VERSION 3.8)

set(NAME NectereClient)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.75.0 REQUIRED system regex)

if(Boost_FOUND)
    include_directories(.)
    include_directories(${PROJECT_SOURCE_DIR}/Nectere)
    include_directories(${Boost_INCLUDE_DIRS}) 
    link_directories(${Boost_LIBRARY_DIRS})

    set(SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
            ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

    add_executable (${NAME} ${SOURCE_FILES})
    if (WIN32)
	    target_link_libraries(${NAME} ${Boost_LIBRARIES})
    else()
	    target_link_libraries(${NAME} ${Boost_LIBRARIES})
    endif()
endif()
