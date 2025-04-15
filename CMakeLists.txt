# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.15)
project(pacmanpp VERSION 0.0.0 LANGUAGES CXX)

set(COMPILE_FLAGS -Wall -Wextra -Wpedantic)

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

if(WARNINGS_AS_ERRORS)
    list(APPEND COMPILE_FLAGS -Werror)
endif()

add_executable(pacmanpp src/main.cc)

target_compile_options(pacmanpp PRIVATE ${COMPILE_FLAGS})
target_compile_features(pacmanpp PRIVATE cxx_std_23)
set_target_properties(pacmanpp PROPERTIES CXX_EXTENSIONS OFF)
