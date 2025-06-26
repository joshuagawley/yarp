# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.23)
project(pacmanpp LANGUAGES CXX)

# Extract version information from version.txt. The first line that looks like
# a version string is used, so the file supports comments
file(STRINGS version.txt version_txt REGEX "^[0-9]+\\.[0-9]+\\.[0-9]+.*$" LIMIT_COUNT 1)

find_package(Git REQUIRED)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --show-toplevel
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  RESULT_VARIABLE git_top_dir_result
  OUTPUT_VARIABLE git_top_dir
  OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

# Set fallback first, override if more information (from git) is found
set(PACMANPP_VERSION "${version_txt}")

# Prevent formatting errors & resolve symlinks (REALPATH)
get_filename_component(resolved_project_source_dir ${PROJECT_SOURCE_DIR} REALPATH)
if(NOT ("${git_top_dir}" STREQUAL ""))
  get_filename_component(git_top_dir ${git_top_dir} REALPATH)
endif()

if(git_top_dir_result EQUAL 0 AND git_top_dir STREQUAL resolved_project_source_dir)
  # If we are in a git repo we can get the version information from git describe
  execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --dirty=-dev
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    RESULT_VARIABLE git_describe_result
    OUTPUT_VARIABLE git_describe
    OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)

  if(git_describe_result EQUAL 0)
    set(PACMANPP_VERSION "${git_describe}")
  else()
    message(STATUS "Could not detect version with git, falling back to built-in version information.")
  endif()

else()
  message(STATUS "CMake and git directory mismatch, falling back to built-in version information.") # Assuming that if git rev-parse doesn't return 0, git describe won't either
endif()


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

# Enhance error reporting and compiler messages
if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
  add_compile_options(-fcolor-diagnostics)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  add_compile_options(-fdiagnostics-color=always)
else()
  message(STATUS "No colored compiler diagnostic set for '${CMAKE_CXX_COMPILER_ID}' compiler.")
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

configure_file(
  ${CMAKE_CURRENT_LIST_DIR}/src/settings.h.in
  ${CMAKE_BINARY_DIR}/src/settings.h
  ESCAPE_QUOTES
)

set(
    PACMANPP_SOURCES
    src/alpm.cc
    src/alpm_package.cc
    src/app.cc
    src/help_handler.cc
    src/main.cc
    src/query_handler.cc
    src/version_handler.cc
)

set(
    PACMANPP_HEADERS
    src/alpm.h
    src/alpm_package.h
    src/app.h
    src/argument_parser.h
    src/bitwise_enum.h
    src/config.h
    src/help_handler.h
    src/operation.h
    src/operation_handler.h
    src/query_handler.h
    src/version_handler.h
    ${CMAKE_BINARY_DIR}/src/settings.h
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

target_include_directories(pacmanpp PRIVATE "${CMAKE_CURRENT_BINARY_DIR}/src")

target_compile_options(pacmanpp PRIVATE ${COMPILE_FLAGS})
target_compile_features(pacmanpp PRIVATE cxx_std_23)
set_target_properties(pacmanpp PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(pacmanpp PRIVATE Alpm::Alpm)

if(BUILD_TESTING)
    find_package(Python3 COMPONENTS Interpreter REQUIRED)
    enable_testing()
    add_subdirectory(tests)
endif()
