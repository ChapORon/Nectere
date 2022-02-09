include_directories(..)

set(LIB_NAME ${NAME}Parameters)
set(NECTERE_DEPENDENCIES)
set(LIB_VERSION_MAJOR 1)
set(LIB_VERSION_MINOR 0)
set(LIB_VERSION_PATCH 0)
set(LIB_VERSION ${LIB_VERSION_MAJOR}.${LIB_VERSION_MINOR}.${LIB_VERSION_PATCH})

set(PARAMETERS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/AuthParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AuthParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/LogInFileParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/LogInFileParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/LogPathParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/LogPathParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ScriptEngineSmartBuildParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ScriptEngineSmartBuildParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/VerboseParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/VerboseParameter.hpp)
	
add_library(${LIB_NAME} ${PARAMETERS_SOURCES})
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

set_target_properties(${LIB_NAME} PROPERTIES
					SOVERSION ${LIB_VERSION_MAJOR}
					VERSION ${LIB_VERSION})

include(GenerateExportHeader)
generate_export_header(${LIB_NAME} EXPORT_FILE_NAME ${CMAKE_CURRENT_SOURCE_DIR}/nectere_parameters_export.h)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
	${LIB_NAME}ConfigVersion.cmake
	VERSION ${LIB_VERSION}
	COMPATIBILITY SameMajorVersion)

include(GNUInstallDirs)
install(TARGETS ${LIB_NAME}
		PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${NAME}/Parameters
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
				COMPONENT ${LIB_NAME}_RunTime
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
				COMPONENT ${LIB_NAME}_RunTime
				NAMELINK_COMPONENT ${NAME}_Development
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
				COMPONENT ${LIB_NAME}_Development)