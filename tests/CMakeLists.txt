# SDPX-License-Identifier: MIT

add_custom_target(copy_files ALL
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pacman_conf.conf
        ${CMAKE_CURRENT_BINARY_DIR}/test_pacman_conf.conf
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test-data
        ${CMAKE_CURRENT_BINARY_DIR}/test-data
)

function(pacmanpp_add_test)
    cmake_parse_arguments(ARG "" "NAME;DESCRIPTION" "" ${ARGN})

    if(NOT ARG_NAME)
        message(FATAL_ERROR "Could not find NAME")
    endif()

     if(NOT ARG_DESCRIPTION)
        message(FATAL_ERROR "Could not find NAME")
    endif()

    add_test(NAME "${ARG_DESCRIPTION}"
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_NAME}.py ${CMAKE_BINARY_DIR}/pacmanpp
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction(pacmanpp_add_test)

pacmanpp_add_test(NAME args001 DESCRIPTION "args001 -- pacmanpp")
pacmanpp_add_test(NAME args002 DESCRIPTION "args002 -- pacmanpp -h")
pacmanpp_add_test(NAME args004 DESCRIPTION "args004 -- pacmanpp --root=/ --dbpath=/var/lib/pacman")
pacmanpp_add_test(NAME args005 DESCRIPTION "args005 -- pacmanpp -v")
pacmanpp_add_test(NAME args007 DESCRIPTION "args007 -- pacmanpp --config test_pacman_conf.conf")
pacmanpp_add_test(NAME query001 DESCRIPTION "query001 -- pacmanpp -Q pacman")
pacmanpp_add_test(NAME query002 DESCRIPTION "query002 -- pacmanpp -Q base")
pacmanpp_add_test(NAME query003 DESCRIPTION "query003 -- pacmanpp -Q bar [doesn't exist]")
pacmanpp_add_test(NAME query004 DESCRIPTION "query004 -- pacmanpp -Qi pacman")
pacmanpp_add_test(NAME query005 DESCRIPTION "query005 -- pacmanpp -Ql alsa-lib")
pacmanpp_add_test(NAME query006 DESCRIPTION "query006 -- pacmanpp -Qg")
pacmanpp_add_test(NAME query007 DESCRIPTION "query007 -- pacmanpp -Qg base-devel")
pacmanpp_add_test(NAME query008 DESCRIPTION "query008 -- pacmanpp -Qd pacman [explicitly installed]")
pacmanpp_add_test(NAME query009 DESCRIPTION "query009 -- pacmanpp -Qd jsoncpp [dependency]")
pacmanpp_add_test(NAME query010 DESCRIPTION "query010 -- pacmanpp -Qe jsoncpp [dependency]")
pacmanpp_add_test(NAME query011 DESCRIPTION "query011 -- pacmanpp -Qe polybar [explicitly installed]")
pacmanpp_add_test(NAME query012 DESCRIPTION "query012 -- pacmanpp -Qk alsa-lib")
pacmanpp_add_test(NAME query013 DESCRIPTION "query013 -- pacmanpp -Qm jsoncpp [in official repo]")
pacmanpp_add_test(NAME query014 DESCRIPTION "query014 -- pacmanpp -Qn jsoncpp [in official repo]")
pacmanpp_add_test(NAME query015 DESCRIPTION "query015 -- pacmanpp -Qp yay-12.5.0-1-x86_64.pkg.tar.zst [package archive]")
pacmanpp_add_test(NAME query016 DESCRIPTION "query016 -- pacmanpp -Qt foo [unrequired]")
pacmanpp_add_test(NAME changelog001 DESCRIPTION "changlog001 -- pacmanpp -Qc powertop")
pacmanpp_add_test(NAME version001 DESCRIPTION "version001 -- pacmanpp -V")



# C++ Unit Tests with Catch2
find_package(Catch2 3 QUIET)
if(Catch2_FOUND)
    add_executable(test_pacman_conf test_pacman_conf.cc)

    target_sources(test_pacman_conf
        PRIVATE
            test_pacman_conf.cc
            ${CMAKE_SOURCE_DIR}/src/pacman_conf.cc
    )

    target_include_directories(test_pacman_conf PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_BINARY_DIR}/src
    )

    target_compile_features(test_pacman_conf PRIVATE cxx_std_23)
    target_compile_options(test_pacman_conf PRIVATE ${COMPILE_FLAGS})

    target_link_libraries(test_pacman_conf PRIVATE Catch2::Catch2WithMain)

    add_custom_command(TARGET test_pacman_conf POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pacman_conf.conf
        ${CMAKE_CURRENT_BINARY_DIR}/test_pacman_conf.conf
    )

    # Register test with CTest
    include(Catch)
    catch_discover_tests(test_pacman_conf)
else()
    message(STATUS "Catch2 not found, skipping C++ unit tests")
endif()
