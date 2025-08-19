set(
        AURPP_SOURCES
        package.cc
)

set(
        AURPP_HEADERS
        package.h
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

target_compile_options(aurpp PRIVATE ${COMPILE_FLAGS})
target_compile_features(aurpp PRIVATE cxx_std_23)
set_target_properties(aurpp PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(aurpp
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(aurpp PRIVATE Jsoncpp::Jsoncpp)