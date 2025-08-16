# SPDX-License-Identifier: MIT

# FindJsoncpp.cmake
# Locates the JsonCpp JSON parsing library
#
# This module defines the following variables:
#   JSONCPP_FOUND        - True if JsonCpp was found
#   JSONCPP_INCLUDE_DIRS - JsonCpp include directory
#   JSONCPP_LIBRARIES    - JsonCpp libraries to link against
#   JSONCPP_VERSION      - The version of JsonCpp found
#
# and the following imported targets:
#   Jsoncpp::Jsoncpp     - The JsonCpp library

# Try to find JsonCpp using pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_JSONCPP QUIET jsoncpp)
endif()

# Set hints for finding the include directory
find_path(JSONCPP_INCLUDE_DIR
  NAMES json/json.h
  PATHS
    ${PC_JSONCPP_INCLUDEDIR}
    ${PC_JSONCPP_INCLUDE_DIRS}
  PATH_SUFFIXES 
    jsoncpp
    json
)

# Find the JsonCpp library
find_library(JSONCPP_LIBRARY
  NAMES jsoncpp libjsoncpp
  PATHS
    ${PC_JSONCPP_LIBDIR}
    ${PC_JSONCPP_LIBRARY_DIRS}
)

# Extract version information from the header file
if(JSONCPP_INCLUDE_DIR)
  file(STRINGS "${JSONCPP_INCLUDE_DIR}/json/version.h" JSONCPP_VERSION_LINE REGEX "^#define[ \t]+JSONCPP_VERSION_STRING[ \t]+\"[^\"]+\".*$")
  string(REGEX REPLACE "^#define[ \t]+JSONCPP_VERSION_STRING[ \t]+\"([^\"]+)\".*$" "\\1" JSONCPP_VERSION "${JSONCPP_VERSION_LINE}")
  unset(JSONCPP_VERSION_LINE)
endif()

# Handle the QUIETLY and REQUIRED arguments and set JSONCPP_FOUND
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Jsoncpp
  REQUIRED_VARS JSONCPP_LIBRARY JSONCPP_INCLUDE_DIR
  VERSION_VAR JSONCPP_VERSION
)

# Set the output variables if JsonCpp was found
if(JSONCPP_FOUND)
  set(JSONCPP_LIBRARIES ${JSONCPP_LIBRARY})
  set(JSONCPP_INCLUDE_DIRS ${JSONCPP_INCLUDE_DIR})
  
  # Create an imported target for JsonCpp
  if(NOT TARGET Jsoncpp::Jsoncpp)
    add_library(Jsoncpp::Jsoncpp UNKNOWN IMPORTED)
    set_target_properties(Jsoncpp::Jsoncpp PROPERTIES
      IMPORTED_LOCATION "${JSONCPP_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${JSONCPP_INCLUDE_DIR}"
    )
  endif()
endif()

# Mark variables as advanced
mark_as_advanced(JSONCPP_INCLUDE_DIR JSONCPP_LIBRARY)