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
            ${CMAKE_CURRENT_SOURCE_DIR}/Nectere.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Nectere.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Discord/Discord.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Discord/Discord.h
            ${CMAKE_CURRENT_SOURCE_DIR}/Twitch/Twitch.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/Twitch/Twitch.h)

    add_executable (${NAME} ${SOURCE_FILES})
    if (WIN32)
	    target_link_libraries(${NAME} ${Boost_LIBRARIES} DataParser-s)
    else()
	    target_link_libraries(${NAME} ${Boost_LIBRARIES} DataParser)
    endif()
endif()
