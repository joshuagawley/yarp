add_subdirectory(alpmpp)
add_subdirectory(aurpp)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/settings.h.in
        ${CMAKE_CURRENT_SOURCE_DIR}/settings.h
        ESCAPE_QUOTES
)

set(
        PACMANPP_SOURCES
        app.cc
        argument_parser.cc
        help_handler.cc
        main.cc
        noop_handler.cc
        pacman_conf.cc
        query_handler.cc
        version_handler.cc
)

set(
        PACMANPP_HEADERS
        app.h
        argument_parser.h
        bitwise_enum.h
        config.h
        help_handler.h
        noop_handler.h
        operation.h
        pacman_conf.h
        query_handler.h
        version_handler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/settings.h
)

add_executable(pacmanpp)

target_sources(
        pacmanpp
        PRIVATE
        ${PACMANPP_SOURCES}

        PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        FILES
        ${PACMANPP_HEADERS}
)

target_compile_options(pacmanpp PRIVATE ${COMPILE_FLAGS})
target_compile_features(pacmanpp PRIVATE cxx_std_23)
set_target_properties(pacmanpp PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(pacmanpp PRIVATE "${CMAKE_SOURCE_DIR}/src")
target_link_libraries(pacmanpp PRIVATE alpmpp)