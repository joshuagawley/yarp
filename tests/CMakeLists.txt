# SDPX-License-Identifier: MIT

add_custom_target(copy_files ALL
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_SOURCE_DIR}/test_pacman_conf.conf
        ${CMAKE_CURRENT_BINARY_DIR}/test_pacman_conf.conf
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/test-data
        ${CMAKE_CURRENT_BINARY_DIR}/test-data
)

add_subdirectory(args)
add_subdirectory(changelog)
add_subdirectory(query)
add_subdirectory(version)

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
