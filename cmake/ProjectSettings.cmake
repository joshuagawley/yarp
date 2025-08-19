include(CompileFlags)

function(pacmanpp_set_project_settings project_settings WARNINGS_AS_ERRORS)
    pacmanpp_set_project_compile_flags(${project_settings} ${WARNINGS_AS_ERRORS})
    target_compile_features(${project_settings} INTERFACE cxx_std_23)
    set_target_properties(${project_settings} PROPERTIES CXX_EXTENSIONS OFF)
    target_include_directories(${project_settings} INTERFACE
            $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>
            $<INSTALL_INTERFACE:include>)
endfunction()