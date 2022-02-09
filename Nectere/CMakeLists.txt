cmake_minimum_required(VERSION 3.19)

set(NAME Nectere)

set(LIB_NAME ${NAME})
set(NECTERE_DEPENDENCIES ${NAME}Dp ${NAME}Parameters ${NAME}Command parg)
set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 0)
set(LIB_VERSION_PATCH 0)
set(LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1")

add_definitions(-DNECTERE_EXPORT_MODE -DPARG_EXPORT_MODE)
add_subdirectory(Concurrency)
add_subdirectory(Network)
add_subdirectory(Dp)
add_subdirectory(Command)
add_subdirectory(Parameters)
add_subdirectory(parg)
#add_subdirectory(Script)

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
	${CMAKE_CURRENT_SOURCE_DIR}/Canal.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Canal.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/CanalManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/CanalManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Configuration.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Configuration.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/DynamicLibrary.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Manager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Manager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/NectereEventCode.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Ptr.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/StringUtils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StringUtils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/UIDSet.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/UserManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/UserManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/VersionHelper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VersionHelper.hpp)

if(NOT APPLE)
	set(CMAKE_INSTALL_RPATH $ORIGIN)
endif()

add_library(${NAME} ${SOURCE_FILES})
target_link_libraries(${NAME} ${NECTERE_DEPENDENCIES})

set_target_properties(${LIB_NAME} PROPERTIES
					SOVERSION ${LIB_VERSION_MAJOR}
					VERSION ${LIB_VERSION})

include(GenerateExportHeader)
generate_export_header(${LIB_NAME} EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/nectere_export.h)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	${LIB_NAME}ConfigVersion.cmake
	VERSION ${LIB_VERSION}
	COMPATIBILITY SameMajorVersion)

include(GNUInstallDirs)
install(TARGETS ${NAME}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${NAME}
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				COMPONENT ${NAME}_RunTime
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				COMPONENT ${NAME}_RunTime
				NAMELINK_COMPONENT ${NAME}_Development
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
				COMPONENT ${NAME}_Development)

set(EXECUTABLE_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_executable(${NAME}-server ${EXECUTABLE_SOURCES})
target_link_libraries(${NAME}-server ${NAME} ${NAME}Network)

set(TEST_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)

add_executable(${NAME}-Test ${TEST_SOURCES})
target_link_libraries(${NAME}-Test ${NAME} ${NAME}Network)