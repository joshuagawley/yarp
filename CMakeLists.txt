# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.23)
project(pacmanpp LANGUAGES CXX)

include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

include(Core)
include(ProjectSettings)

add_library(project_settings INTERFACE)
pacmanpp_set_project_settings(project_settings WARNING_AS_ERRORS)

find_package(Alpm REQUIRED)
find_package(Jsoncpp REQUIRED)

add_subdirectory(src)

if (BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    find_package(Catch2 3 REQUIRED)

    include(Catch)
    include(TestUtils)

    enable_testing()

    add_subdirectory(tests)
endif ()
