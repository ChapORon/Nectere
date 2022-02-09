cmake_minimum_required(VERSION 3.19)
project(NectereModules)

set(CMAKE_CXX_STANDARD 20)

set(NAME TemplateModule)

find_package(Nectere 1.0)

set(SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Module.cpp)

add_library(${NAME} MODULE ${SOURCE_FILES})
target_link_libraries(${NAME} ${Nectere_LIBRAIRIES})