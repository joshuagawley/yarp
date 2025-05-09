# SPDX-License-Identifier: MIT

# FindAlpm.cmake
# Locates the ALPM (Arch Linux Package Management) library
#
# This module defines the following variables:
#   ALPM_FOUND        - True if ALPM was found
#   ALPM_INCLUDE_DIRS - ALPM include directory
#   ALPM_LIBRARIES    - ALPM libraries to link against
#   ALPM_VERSION      - The version of ALPM found
#
# and the following imported targets:
#   Alpm::Alpm        - The ALPM library

# Try to find ALPM using pkg-config first
find_package(PkgConfig QUIET)
if(PKG_CONFIG_FOUND)
  pkg_check_modules(PC_ALPM QUIET libalpm)
endif()

# Set hints for finding the include directory
find_path(ALPM_INCLUDE_DIR
  NAMES alpm.h
  PATHS
    ${PC_ALPM_INCLUDEDIR}
    ${PC_ALPM_INCLUDE_DIRS}
  PATH_SUFFIXES 
    alpm
    libalpm
)

# Find the ALPM library
find_library(ALPM_LIBRARY
  NAMES alpm libalpm
  PATHS
    ${PC_ALPM_LIBDIR}
    ${PC_ALPM_LIBRARY_DIRS}
)

# Extract version information from the header file
if(ALPM_INCLUDE_DIR)
  file(STRINGS "${ALPM_INCLUDE_DIR}/alpm.h" ALPM_VERSION_LINE REGEX "^#define[ \t]+ALPM_VERSION[ \t]+\"[^\"]+\".*$")
  string(REGEX REPLACE "^#define[ \t]+ALPM_VERSION[ \t]+\"([^\"]+)\".*$" "\\1" ALPM_VERSION "${ALPM_VERSION_LINE}")
  unset(ALPM_VERSION_LINE)
endif()

# Handle the QUIETLY and REQUIRED arguments and set ALPM_FOUND
include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Alpm
  REQUIRED_VARS ALPM_LIBRARY ALPM_INCLUDE_DIR
  VERSION_VAR ALPM_VERSION
)

# Set the output variables if ALPM was found
if(ALPM_FOUND)
  set(ALPM_LIBRARIES ${ALPM_LIBRARY})
  set(ALPM_INCLUDE_DIRS ${ALPM_INCLUDE_DIR})
  
  # Create an imported target for ALPM
  if(NOT TARGET Alpm::Alpm)
    add_library(Alpm::Alpm UNKNOWN IMPORTED)
    set_target_properties(Alpm::Alpm PROPERTIES
      IMPORTED_LOCATION "${ALPM_LIBRARY}"
      INTERFACE_INCLUDE_DIRECTORIES "${ALPM_INCLUDE_DIR}"
    )
  endif()
endif()

# Mark variables as advanced
mark_as_advanced(ALPM_INCLUDE_DIR ALPM_LIBRARY)