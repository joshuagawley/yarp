add_subdirectory(alpmpp)
add_subdirectory(aurpp)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/settings.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/settings.h
        ESCAPE_QUOTES
)

set(
        YARP_SOURCES
        app.cc
        argument_parser.cc
        help_handler.cc
        main.cc
        noop_handler.cc
        pacman_conf.cc
        query_handler.cc
        sync_handler.cc
        utils.cc
        version_handler.cc
)

set(
        YARP_HEADERS
        app.h
        argument_parser.h
        bitwise_enum.h
        config.h
        help_handler.h
        noop_handler.h
        operation.h
        pacman_conf.h
        query_handler.h
        sync_handler.h
        utils.h
        version_handler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/settings.h
)

add_executable(yarp)

target_sources(
        yarp
        PRIVATE
        ${YARP_SOURCES}

        PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        FILES
        ${YARP_HEADERS}
)

target_link_libraries(yarp PRIVATE project_settings alpmpp aurpp)