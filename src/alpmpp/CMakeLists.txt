set(ALPMPP_SOURCES
        alpm.cc
        depend.cc
        package.cc
        util.h
)

set(ALPMPP_HEADERS
        alpm.h
        bitwise_enum.h
        depend.h
        file.h
        package.h
        types.h
        util.h
)

add_library(alpmpp)

target_sources(
        alpmpp
        PRIVATE
        ${ALPMPP_SOURCES}

        PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/..
        FILES
        ${ALPMPP_HEADERS}
)

target_link_libraries(alpmpp PUBLIC project_settings Alpm::Alpm)