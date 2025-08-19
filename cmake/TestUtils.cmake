function(pacmanpp_add_test)
    cmake_parse_arguments(ARG "" "NAME;DESCRIPTION" "" ${ARGN})

    if (NOT ARG_NAME)
        message(FATAL_ERROR "Could not find NAME")
    endif ()

    if (NOT ARG_DESCRIPTION)
        message(FATAL_ERROR "Could not find NAME")
    endif ()

    add_test(NAME "${ARG_DESCRIPTION}"
            COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/${ARG_NAME}.py ${CMAKE_BINARY_DIR}/src/pacmanpp
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endfunction(pacmanpp_add_test)

function(pacmanpp_add_unit_test)
    cmake_parse_arguments(ARG "" "NAME" "SOURCES;COPY_FILES;LIBRARIES" ${ARGN})

    if (NOT ARG_NAME)
        message(FATAL_ERROR "Could not find NAME for C++ test")
    endif ()

    if (NOT ARG_SOURCES)
        message(FATAL_ERROR "Could not find SOURCES for C++ test")
    endif ()

    add_executable(${ARG_NAME} ${ARG_SOURCES})

    target_sources(${ARG_NAME}
            PRIVATE
            ${ARG_SOURCES}
    )

    target_include_directories(${ARG_NAME} PRIVATE
            ${CMAKE_SOURCE_DIR}/src
            ${CMAKE_BINARY_DIR}/src
    )

    # Link against Catch2 and any additional libraries in a single call
    target_link_libraries(${ARG_NAME} PRIVATE project_settings Catch2::Catch2WithMain ${ARG_LIBRARIES})

    # Copy any specified files to the build directory
    if (ARG_COPY_FILES)
        foreach (file ${ARG_COPY_FILES})
            add_custom_command(TARGET ${ARG_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy
                    ${CMAKE_CURRENT_SOURCE_DIR}/${file}
                    ${CMAKE_CURRENT_BINARY_DIR}/${file}
            )
        endforeach ()
    endif ()

    catch_discover_tests(${ARG_NAME})
endfunction(pacmanpp_add_unit_test)
