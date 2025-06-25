# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.23)
project(pacmanpp VERSION 0.0.0 LANGUAGES CXX)

include(CTest)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set a default build type if none was specified
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to 'RelWithDebInfo' as none was specified.")
  set(CMAKE_BUILD_TYPE
      RelWithDebInfo
      CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui, ccmake
  set_property(
    CACHE CMAKE_BUILD_TYPE
    PROPERTY STRINGS
             "Debug"
             "Release"
             "MinSizeRel"
             "RelWithDebInfo")
endif()

set(COMPILE_FLAGS
  -Wall
  -Wextra # reasonable and standard
  -Wshadow # warn the user if a variable declaration shadows one from a parent context
  -Wnon-virtual-dtor # warn the user if a class with virtual functions has a non-virtual destructor. This helps
  # catch hard to track down memory errors
  -Wold-style-cast # warn for c-style casts
  -Wcast-align # warn for potential performance problem casts
  -Wunused # warn on anything being unused
  -Woverloaded-virtual # warn if you overload (not override) a virtual function
  -Wpedantic # warn if non-standard C++ is used
  -Wconversion # warn on type conversions that may lose data
  -Wsign-conversion # warn on sign conversions
  -Wnull-dereference # warn if a null dereference is detected
  -Wdouble-promotion # warn if float is implicit promoted to double
  -Wformat=2 # warn on security issues around functions that format output (ie printf)
  -Wimplicit-fallthrough # warn on statements that fallthrough without an explicit annotation
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  list(APPEND
    COMPILE_FLAGS
    -Wmisleading-indentation # warn if indentation implies blocks where blocks do not exist
    -Wduplicated-cond # warn if if / else chain has duplicated conditions
    -Wduplicated-branches # warn if if / else branches have duplicated code
    -Wlogical-op # warn about logical operations being used where bitwise were probably wanted
    -Wuseless-cast # warn if you perform a cast to the same type
    -Wsuggest-override # warn if an overridden member function is not marked 'override' or 'final'
  )
endif()

option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)
if(WARNINGS_AS_ERRORS)
  message(TRACE "Warnings are treated as errors")
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
target_compile_features(pacmanpp PRIVATE cxx_std_23)
set_target_properties(pacmanpp PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(pacmanpp PRIVATE Alpm::Alpm)

if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()
