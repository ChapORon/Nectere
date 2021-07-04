cmake_minimum_required(VERSION 3.19)

set(NAME Nectere)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG=1")

include_directories(.)
	
set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.76.0 REQUIRED system regex date_time)

set(DP_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Dp/Format.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Dp/Json.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dp/Json.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dp/Node.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dp/Node.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dp/Result.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dp/Result.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dp/Serializer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dp/Xml.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Dp/Xml.hpp)

set(COMMAND_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Command/ANectereCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Command/ANectereCommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Command/StopCommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Command/StopCommand.hpp)

set(NETWORK_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Network/AServer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/Boost_Server.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/Boost_Server.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/Boost_Session.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/Boost_Session.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/Header.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/IEventReceiver.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/Windows_Server.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/Windows_Server.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/Windows_Session.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Network/Windows_Session.hpp)

set(PARAMETERS_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/LogInFileParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/LogInFileParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/LogPathParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/LogPathParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/NetworkPortParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/NetworkPortParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/ScriptEngineSmartBuildParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/ScriptEngineSmartBuildParameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/VerboseParameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Parameters/VerboseParameter.hpp)

set(SCRIPT_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Script/NativeFunction/DeclaredObjectFunction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/NativeFunction/RawFunction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/NativeFunction/VoidFunction.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/NativeFunction/VoidFunction.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/ArrayTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/ArrayTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/BoolTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/BoolTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/CharTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/CharTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/DoubleTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/DoubleTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/FloatTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/FloatTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/IntTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/IntTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/LongDoubleTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/LongDoubleTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/LongLongTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/LongLongTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/LongTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/LongTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/ObjectTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/ObjectTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/ShortTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/ShortTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/StringTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/StringTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/Typer.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/TyperArray.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/TyperArray.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedCharTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedCharTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedIntTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedIntTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedLongLongTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedLongLongTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedLongTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedLongTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedShortTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/UnsignedShortTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/VoidTyper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/UnitTyper/VoidTyper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/ACaster.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Callable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Callable.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/CallStack.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/CallStack.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Caster.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Caster.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/CodingStyle.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/CodingStyle.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/CurrentEnvironment.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/CurrentEnvironment.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/DeclaredObjectDefinition.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/DeclaredObjectDefinition.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/EVariableType.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Function.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Function.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/ILinkerWrapper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Interpreter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Interpreter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/IResolutionTreeOperation.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Linker.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/LinkerWrapper.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/LocalScope.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/LocalScope.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Native.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Native.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/NativeCaster.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/NativeStringCaster.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/NativeStringCaster.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/ObjectLinker.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Parameter.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Parameter.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/PrimitiveLinker.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/ResolutionTree.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/ResolutionTree.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/ResolutionTreeUtils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/ResolutionTreeUtils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Script.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Script.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/ScriptLoader.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/ScriptLoader.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Signature.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Signature.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/SingleScriptParser.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/SingleScriptParser.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/StringLinker.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/SubTypeLinker.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Tag.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Taggable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Taggable.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Unit.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Unit.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Variable.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/Variable.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/VariableDefinition.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/VariableType.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/VariableType.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/VariableTypeHelper.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Script/VariableTypeHelper.hpp)

set(PARG_SOURCES
	${CMAKE_CURRENT_SOURCE_DIR}/parg.h
	${CMAKE_CURRENT_SOURCE_DIR}/parg.c
	${CMAKE_CURRENT_SOURCE_DIR}/parg_def.h)

set(SOURCE_FILES
	${PARG_SOURCES} ${COMMAND_SOURCES} ${PARAMETERS_SOURCES} ${SCRIPT_FILES} ${DP_SOURCES} ${NETWORK_SOURCES}
	${CMAKE_CURRENT_SOURCE_DIR}/AApplicationHandler.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/AApplicationHandler.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ACommand.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ACommand.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Application.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ApplicationManager.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ApplicationManager.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Configuration.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Configuration.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/DynamicLibrary.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Event.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/IDGenerator.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Logger.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Logger.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/NectereEventCode.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/StringUtils.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/StringUtils.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Task.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Task.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/TaskResult.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Thread.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Thread.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThreadSystem.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/ThreadSystem.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/UIDSet.hpp)
	
set_source_files_properties(${PARG_SOURCES} PROPERTIES LANGUAGE CXX)

add_executable(${NAME} ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
add_executable(${NAME}-Test ${SOURCE_FILES} ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp)
add_library(${NAME}-s STATIC ${SOURCE_FILES})

if (Boost_FOUND)
	add_definitions(-DUSE_BOOST)
    include_directories(${Boost_INCLUDE_DIRS})
    link_directories(${Boost_LIBRARY_DIRS})
    target_link_libraries(${NAME} ${Boost_LIBRARIES})
    target_link_libraries(${NAME}-Test ${Boost_LIBRARIES})
endif()