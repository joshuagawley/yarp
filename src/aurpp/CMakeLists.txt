set(
        AURPP_SOURCES
        package.cc
        request.cc
)

set(
        AURPP_HEADERS
        package.h
        request.h
)

add_library(aurpp)

target_sources(
        aurpp
        PRIVATE
        ${AURPP_SOURCES}

        PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        FILES
        ${AURPP_HEADERS}
)

target_link_libraries(aurpp PRIVATE project_settings CURL::libcurl Jsoncpp::Jsoncpp)