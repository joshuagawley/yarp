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
pacmanpp_add_test(NAME args004 DESCRIPTION "Accepts custom root and database paths with --root and --dbpath arguments")
pacmanpp_add_test(NAME args005 DESCRIPTION "Outputs root and dbpath when short verbose flag is used")
pacmanpp_add_test(NAME args006 DESCRIPTION "Outputs root and dbpath when long verbose flag is used")
pacmanpp_add_test(NAME query001 DESCRIPTION "Outputs package name and description when query flag is given")
pacmanpp_add_test(NAME query002 DESCRIPTION "Outputs group when query flag is given and installed group is passed in")
pacmanpp_add_test(NAME query003 DESCRIPTION "Outputs correct error message when querying for not installed packages")
pacmanpp_add_test(NAME query004 DESCRIPTION "Outputs correct package information when querying for package information")
pacmanpp_add_test(NAME query005 DESCRIPTION "Outputs package file list when querying for package file list")
pacmanpp_add_test(NAME query006 DESCRIPTION "Outputs all group information when no target group is passed in")
pacmanpp_add_test(NAME query007 DESCRIPTION "Outputs specific group information when target group is passed in")
pacmanpp_add_test(NAME query008 DESCRIPTION "Correct output when querying for explicitly installed package with deps filter")
pacmanpp_add_test(NAME query009 DESCRIPTION "Correct output when querying for package installed as dependency with deps filter")
pacmanpp_add_test(NAME query010 DESCRIPTION "Correct output when querying for explicitly installed package with explicit filter")
pacmanpp_add_test(NAME query011 DESCRIPTION "Correct output when querying for package installed as dependency with explicit filter")
pacmanpp_add_test(NAME changelog001 DESCRIPTION "Outputs changelog when short changelog flag is given")
pacmanpp_add_test(NAME changelog002 DESCRIPTION "Outputs changelog when long changelog flag is given")
pacmanpp_add_test(NAME version001 DESCRIPTION "Outputs version information when short version flag is given")
pacmanpp_add_test(NAME version002 DESCRIPTION "Outputs version information when long version flag is given")
