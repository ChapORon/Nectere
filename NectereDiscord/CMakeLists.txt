cmake_minimum_required(VERSION 3.19)

set(NAME NectereDiscord)

if (WIN32)
    set(Boost_USE_STATIC_LIBS ON)
else()
    set(Boost_USE_STATIC_LIBS OFF)
endif()
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.75.0 REQUIRED system regex)

if (Boost_FOUND)
    include_directories(.)
    include_directories(${PROJECT_SOURCE_DIR})
    include_directories(${Boost_INCLUDE_DIRS}) 
    link_directories(${Boost_LIBRARY_DIRS})

    set(SOURCE_FILES
            ${CMAKE_CURRENT_SOURCE_DIR}/Discord.cpp)

    add_library(${NAME} SHARED ${SOURCE_FILES})
    if (WIN32)
	    target_link_libraries(${NAME} ${Boost_LIBRARIES} Nectere-s)
    else()
	    target_link_libraries(${NAME} ${Boost_LIBRARIES} Nectere)
    endif()
endif()
