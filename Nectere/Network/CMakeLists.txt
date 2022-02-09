include_directories(..)

set(LIB_NAME ${NAME}Network)
set(NECTERE_DEPENDENCIES ${NAME}Concurrency ${NAME}Parameters parg)
set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 0)
set(LIB_VERSION_PATCH 0)
set(LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.76.0 REQUIRED system regex date_time)

set(NETWORK_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/AServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AServer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/BoostNetworkServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/BoostNetworkServer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/BoostNetworkUser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/BoostNetworkUser.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Header.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Helper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Helper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/IEventReceiver.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/NetworkPortParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/NetworkPortParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/NetworkWhitelistParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/NetworkWhitelistParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Server.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Server.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/WindowsNetworkServer.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/WindowsNetworkServer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/WindowsNetworkUser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/WindowsNetworkUser.hpp)

add_library(${LIB_NAME} ${NETWORK_SOURCES} "AServer.cpp")

if (Boost_FOUND)
	add_definitions(-DUSE_BOOST)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries(${LIB_NAME} ${Boost_LIBRARIES})
endif()

target_link_libraries(${LIB_NAME} ${NECTERE_DEPENDENCIES} ws2_32)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})