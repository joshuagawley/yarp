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

pacmanpp_add_test(NAME args001 DESCRIPTION "args001 -- pacmanpp")
pacmanpp_add_test(NAME args002 DESCRIPTION "args002 -- pacmanpp -h")
pacmanpp_add_test(NAME args004 DESCRIPTION "args004 -- pacmanpp --root=/ --dbpath=/var/lib/pacman")
pacmanpp_add_test(NAME args005 DESCRIPTION "args005 -- pacmanpp -v")
pacmanpp_add_test(NAME query001 DESCRIPTION "query001 -- pacmanpp -Q foo")
pacmanpp_add_test(NAME query002 DESCRIPTION "query002 -- pacmanpp -Q base")
pacmanpp_add_test(NAME query003 DESCRIPTION "query003 -- pacmanpp --root `tmp_root` --dbpath `tmp_db` -Q bar [doesn't exist]")
pacmanpp_add_test(NAME query004 DESCRIPTION "query004 -- pacmanpp -Qi powertop")
pacmanpp_add_test(NAME query005 DESCRIPTION "query005 -- pacmanpp -Ql powertop")
pacmanpp_add_test(NAME query006 DESCRIPTION "query006 -- pacmanpp -Qg")
pacmanpp_add_test(NAME query007 DESCRIPTION "query007 -- pacmanpp -Qg nerd-fonts")
pacmanpp_add_test(NAME query008 DESCRIPTION "query008 -- pacmanpp -Qd powertop [explicitly installed]")
pacmanpp_add_test(NAME query009 DESCRIPTION "query009 -- pacmanpp -Qd jsoncpp [dependency]")
pacmanpp_add_test(NAME query010 DESCRIPTION "query010 -- pacmanpp -Qe jsoncpp [dependency]")
pacmanpp_add_test(NAME query011 DESCRIPTION "query011 -- pacmanpp -Qe powertop [explicitly installed]")
pacmanpp_add_test(NAME query012 DESCRIPTION "query012 -- pacmanpp -Qk powertop")
pacmanpp_add_test(NAME query013 DESCRIPTION "query013 -- pacmanpp -Qm jsoncpp [in official repo]")
pacmanpp_add_test(NAME query014 DESCRIPTION "query014 -- pacmanpp -Qn jsoncpp [in official repo]")
pacmanpp_add_test(NAME query015 DESCRIPTION "query015 -- pacmanpp -Qp yay-12.5.0-1-x86_64.pkg.tar.zst [package archive]")
pacmanpp_add_test(NAME changelog001 DESCRIPTION "changlog001 -- pacmanpp -Qc powertop")
pacmanpp_add_test(NAME version001 DESCRIPTION "version001 -- pacmanpp -V")
