# SDPX-License-Identifier: MIT

add_custom_target(copy_files ALL
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pacman_conf.conf
        ${CMAKE_CURRENT_BINARY_DIR}/test_pacman_conf.conf
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test-data
        ${CMAKE_CURRENT_BINARY_DIR}/test-data
)

yarp_add_test(NAME args001 DESCRIPTION "args001 -- yarp")
yarp_add_test(NAME args002 DESCRIPTION "args002 -- yarp -h")
yarp_add_test(NAME args004 DESCRIPTION "args004 -- yarp --root=/ --dbpath=/var/lib/pacman")
yarp_add_test(NAME args005 DESCRIPTION "args005 -- yarp -v")
yarp_add_test(NAME args007 DESCRIPTION "args007 -- yarp --config test_pacman_conf.conf")
yarp_add_test(NAME query001 DESCRIPTION "query001 -- yarp -Q pacman")
yarp_add_test(NAME query002 DESCRIPTION "query002 -- yarp -Q base")
yarp_add_test(NAME query003 DESCRIPTION "query003 -- yarp -Q bar [doesn't exist]")
yarp_add_test(NAME query004 DESCRIPTION "query004 -- yarp -Qi pacman")
yarp_add_test(NAME query005 DESCRIPTION "query005 -- yarp -Ql alsa-lib")
yarp_add_test(NAME query006 DESCRIPTION "query006 -- yarp -Qg")
yarp_add_test(NAME query007 DESCRIPTION "query007 -- yarp -Qg base-devel")
yarp_add_test(NAME query008 DESCRIPTION "query008 -- yarp -Qd pacman [explicitly installed]")
yarp_add_test(NAME query009 DESCRIPTION "query009 -- yarp -Qd jsoncpp [dependency]")
yarp_add_test(NAME query010 DESCRIPTION "query010 -- yarp -Qe jsoncpp [dependency]")
yarp_add_test(NAME query011 DESCRIPTION "query011 -- yarp -Qe polybar [explicitly installed]")
yarp_add_test(NAME query012 DESCRIPTION "query012 -- yarp -Qk alsa-lib")
yarp_add_test(NAME query013 DESCRIPTION "query013 -- yarp -Qm jsoncpp [in official repo]")
yarp_add_test(NAME query014 DESCRIPTION "query014 -- yarp -Qn jsoncpp [in official repo]")
yarp_add_test(NAME query015 DESCRIPTION "query015 -- yarp -Qp yay-12.5.0-1-x86_64.pkg.tar.zst [package archive]")
yarp_add_test(NAME query016 DESCRIPTION "query016 -- yarp -Qt foo [unrequired]")
yarp_add_test(NAME query017 DESCRIPTION "query017 -- yarp -Qu cmake [upgrade]")
yarp_add_test(NAME query018 DESCRIPTION "query018 -- yarp -Qo pacman [file exists]")
yarp_add_test(NAME query019 DESCRIPTION "query019 -- yarp -Qo bar [file doesn't exists]")
yarp_add_test(NAME query020 DESCRIPTION "query020 -- yarp -Qs pacman [exists in local database]")
yarp_add_test(NAME query021 DESCRIPTION "query020 -- yarp -Qs yarp [doesn't exist in local database]")
yarp_add_test(NAME changelog001 DESCRIPTION "changlog001 -- yarp -Qc powertop")
yarp_add_test(NAME version001 DESCRIPTION "version001 -- yarp -V")

yarp_add_unit_test(
        NAME test_pacman_conf
        SOURCES
        test_pacman_conf.cc
        ${CMAKE_SOURCE_DIR}/src/pacman_conf.cc
        COPY_FILES
        test_pacman_conf.conf
)
yarp_add_unit_test(
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
