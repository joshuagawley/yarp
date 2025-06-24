# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.23)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(pacmanpp VERSION 0.0.0 LANGUAGES CXX)

include(cmake/PreventInSourceBuilds.cmake)

include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(COMPILE_FLAGS -Wall -Wextra -Wpedantic)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if(WARNINGS_AS_ERRORS)
    list(APPEND COMPILE_FLAGS -Werror)
endif()

find_package(Alpm REQUIRED)

set(
    PACMANPP_SOURCES 
    src/alpm.cc
    src/alpm_package.cc
    src/app.cc 
    src/main.cc
)

set(
    PACMANPP_HEADERS
    src/alpm.h
    src/alpm_package.h
    src/app.h
    src/argument_parser.h
    src/bitwise_enum.h
    src/config.h
    src/operation.h
)

add_executable(pacmanpp)

target_sources(
    pacmanpp
    PRIVATE
        ${PACMANPP_SOURCES}
    
    PUBLIC
    FILE_SET HEADERS
    TYPE HEADERS
    FILES
        ${PACMANPP_HEADERS}
)

target_compile_options(pacmanpp PRIVATE ${COMPILE_FLAGS})
target_link_libraries(pacmanpp PRIVATE Alpm::Alpm)

if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()