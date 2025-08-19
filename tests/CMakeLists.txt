# SDPX-License-Identifier: MIT

add_custom_target(copy_files ALL
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pacman_conf.conf
        ${CMAKE_CURRENT_BINARY_DIR}/test_pacman_conf.conf
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test-data
        ${CMAKE_CURRENT_BINARY_DIR}/test-data
)

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
pacmanpp_add_test(NAME query017 DESCRIPTION "query017 -- pacmanpp -Qu cmake [upgrade]")
pacmanpp_add_test(NAME query018 DESCRIPTION "query018 -- pacmanpp -Qo pacman [file exists]")
pacmanpp_add_test(NAME query019 DESCRIPTION "query019 -- pacmanpp -Qo bar [file doesn't exists]")
pacmanpp_add_test(NAME query020 DESCRIPTION "query020 -- pacmanpp -Qs pacman [exists in local database]")
pacmanpp_add_test(NAME query021 DESCRIPTION "query020 -- pacmanpp -Qs pacmanpp [doesn't exist in local database]")
pacmanpp_add_test(NAME changelog001 DESCRIPTION "changlog001 -- pacmanpp -Qc powertop")
pacmanpp_add_test(NAME version001 DESCRIPTION "version001 -- pacmanpp -V")

pacmanpp_add_unit_test(
        NAME test_pacman_conf
        SOURCES
        test_pacman_conf.cc
        ${CMAKE_SOURCE_DIR}/src/pacman_conf.cc
        COPY_FILES
        test_pacman_conf.conf
)
pacmanpp_add_unit_test(
        NAME test_aur_package
        SOURCES
        test_aur_package.cc
        ${CMAKE_SOURCE_DIR}/src/aurpp/package.cc
        COPY_FILES
        paru.json
        LIBRARIES
        aurpp
        Jsoncpp::Jsoncpp
)
