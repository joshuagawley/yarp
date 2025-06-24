include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

macro(pacmanpp_supports_sanitizers)
  message(STATUS "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform")
  set(TEST_PROGRAM "int main() { return 0; }")
  # Check if UndefinedBehaviorSanitizer works at link time
  set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
  set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
  check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)
  if(HAS_UBSAN_LINK_SUPPORT)
    message(STATUS "UndefinedBehaviorSanitizer is supported at both compile and link time.")
    set(SUPPORTS_UBSAN ON)
  else()
    message(WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
    set(SUPPORTS_UBSAN OFF)
  endif()

  message(STATUS "Sanity checking AddressSanitizer, it should be supported on this platform")
  set(TEST_PROGRAM "int main() { return 0; }")
  # Check if AddressSanitizer works at link time
  set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
  set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
  check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)
  if(HAS_ASAN_LINK_SUPPORT)
    message(STATUS "AddressSanitizer is supported at both compile and link time.")
    set(SUPPORTS_ASAN ON)
  else()
    message(WARNING "AddressSanitizer is NOT supported at link time.")
    set(SUPPORTS_ASAN OFF)
  endif()
endmacro()

macro(pacmanpp_setup_options)
  option(PACMANPP_ENABLE_HARDENING "Enable hardening" OFF)
  option(PACMANPP_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    PACMANPP_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    PACMANPP_ENABLE_HARDENING
    OFF)

  pacmanpp_supports_sanitizers()

  if(PACMANPP_PACKAGING_MAINTAINER_MODE)
    option(PACMANPP_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(PACMANPP_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(PACMANPP_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(PACMANPP_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(PACMANPP_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(PACMANPP_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(PACMANPP_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(PACMANPP_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(PACMANPP_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(PACMANPP_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(PACMANPP_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(PACMANPP_ENABLE_PCH "Enable precompiled headers" OFF)
    option(PACMANPP_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(PACMANPP_ENABLE_IPO "Enable IPO/LTO" ON)
    option(PACMANPP_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(PACMANPP_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(PACMANPP_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" ${SUPPORTS_ASAN})
    option(PACMANPP_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(PACMANPP_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" ${SUPPORTS_UBSAN})
    option(PACMANPP_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(PACMANPP_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(PACMANPP_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(PACMANPP_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(PACMANPP_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(PACMANPP_ENABLE_PCH "Enable precompiled headers" OFF)
    option(PACMANPP_ENABLE_CACHE "Enable ccache" ON)
  endif()
endmacro()

macro(pacmanpp_global_options)
  if(PACMANPP_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    pacmanpp_enable_ipo()
  endif()

  pacmanpp_supports_sanitizers()

  if(PACNAMPP_ENABLE_HARDENING AND PACMANPP_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN 
       OR PACMANPP_ENABLE_SANITIZER_UNDEFINED
       OR PACMANPP_ENABLE_SANITIZER_ADDRESS
       OR PACMANPP_ENABLE_SANITIZER_THREAD
       OR PACMANPP_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    message("${PACMANPP_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${PACMANPP_ENABLE_SANITIZER_UNDEFINED}")
    pacmanpp_enable_hardening(pacmanpp_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(pacmanpp_local_options)
  include(cmake/StandardProjectSettings.cmake)

  add_library(pacmanpp_warnings INTERFACE)
  add_library(pacmanpp_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  pacmanpp_set_project_warnings(
    pacmanpp_warnings
    ${PACMANPP_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(PACMANPP_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    pacmanpp_configure_linker(pacmanpp_options)
  endif()

  include(cmake/Sanitizers.cmake)
  pacmanpp_enable_sanitizers(
    pacmanpp_options
    ${PACMANPP_ENABLE_SANITIZER_ADDRESS}
    ${PACMANPP_ENABLE_SANITIZER_LEAK}
    ${PACMANPP_ENABLE_SANITIZER_UNDEFINED}
    ${PACMANPP_ENABLE_SANITIZER_THREAD}
    ${PACMANPP_ENABLE_SANITIZER_MEMORY})

  set_target_properties(pacmanpp_options PROPERTIES UNITY_BUILD ${PACMANPP_ENABLE_UNITY_BUILD})

  if(PACMANPP_ENABLE_PCH)
    target_precompile_headers(
      pacmanpp_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(PACMANPP_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    pacmanpp_enable_cache()
  endif()

  include(cmake/StaticAnalyzers.cmake)
  if(PACMANPP_ENABLE_CLANG_TIDY)
    pacmanpp_enable_clang_tidy(pacmanpp_options ${PACMANPP_WARNINGS_AS_ERRORS})
  endif()

  if(PACMANPP_ENABLE_CPPCHECK)
    pacmanpp_enable_cppcheck(${PACMANPP_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(PACMANPP_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    pacmanpp_enable_coverage(pacmanpp_options)
  endif()

  if(PACMANPP_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)
    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(pacmanpp_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

#   if(PACMANPP_ENABLE_HARDENING AND NOT PACMANPP_ENABLE_GLOBAL_HARDENING)
#     include(cmake/Hardening.cmake)
#     if(NOT SUPPORTS_UBSAN 
#        OR PACMANPP_ENABLE_SANITIZER_UNDEFINED
#        OR PACMANPP_ENABLE_SANITIZER_ADDRESS
#        OR PACMANPP_ENABLE_SANITIZER_THREAD
#        OR PACMANPP_ENABLE_SANITIZER_LEAK)
#       set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
#     else()
#       set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
#     endif()
#     pacmanpp_enable_hardening(pacmanpp_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
#   endif()

endmacro()
