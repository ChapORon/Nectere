include_directories(..)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.76.0 REQUIRED system regex date_time)

set(NETWORK_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/AServer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/BoostNetworkServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/BoostNetworkServer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/BoostNetworkUser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/BoostNetworkUser.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Header.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/IEventReceiver.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/WindowsNetworkServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/WindowsNetworkServer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/WindowsNetworkUser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/WindowsNetworkUser.hpp)

add_library(${NAME}-network ${NETWORK_SOURCES})

if (Boost_FOUND)
	add_definitions(-DUSE_BOOST)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries(${NAME}-network ${Boost_LIBRARIES})
endif()

target_link_libraries(${NAME}-network ${NAME}-concurrency ws2_32)
target_include_directories(${NAME}-network PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})