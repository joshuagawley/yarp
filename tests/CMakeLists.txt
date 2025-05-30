# SDPX-License-Identifier: MIT

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

pacmanpp_add_test(NAME args001 DESCRIPTION "Outputs errors when no arguments are given")
pacmanpp_add_test(NAME args002 DESCRIPTION "Outputs usage when help flag is given")
pacmanpp_add_test(NAME args003 DESCRIPTION "Outputs usage when long help flag is given")
pacmanpp_add_test(NAME args004 DESCRIPTION "Accepts custom root path with --root argument")
pacmanpp_add_test(NAME args004 DESCRIPTION "Accepts custom database path with --dbpath argument")
pacmanpp_add_test(NAME query001 DESCRIPTION "Outputs package name and description when query flag is given")
pacmanpp_add_test(NAME query002 DESCRIPTION "Outputs group when query flag is given and installed group is passed in")
pacmanpp_add_test(NAME query003 DESCRIPTION "Outputs correct error message when querying for not installed packages")
