# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.23)

if(NOT DEFINED CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
endif()

set(CMAKE_CXX_EXTENSIONS OFF)

project(pacmanpp VERSION 0.0.0 LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(cmake/PreventInSourceBuilds.cmake)
include(ProjectOptions.cmake)
include(CTest)

pacmanpp_setup_options()
pacmanpp_global_options()
pacmanpp_local_options()

target_compile_features(pacmanpp_options INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

add_library(pacmanpp::pacmanpp_options ALIAS pacmanpp_options)
add_library(pacmanpp::pacmanpp_warnings ALIAS pacmanpp_warnings)

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

target_link_libraries(pacmanpp PRIVATE pacmanpp::pacmanpp_options pacmanpp::pacmanpp_warnings Alpm::Alpm)

if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()