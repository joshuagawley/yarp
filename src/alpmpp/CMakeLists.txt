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

target_compile_options(alpmpp PRIVATE ${COMPILE_FLAGS})
target_compile_features(alpmpp PRIVATE cxx_std_23)
set_target_properties(alpmpp PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(alpmpp 
        PUBLIC 
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(alpmpp PUBLIC Alpm::Alpm)