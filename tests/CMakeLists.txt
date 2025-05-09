# SDPX-License-Identifier: MIT

add_test(NAME "Outputs error when no arguments are given"
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/args001.py ${CMAKE_BINARY_DIR}/pacmanpp
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         )

add_test(NAME "Outputs usage when help flag is given"
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/args002.py ${CMAKE_BINARY_DIR}/pacmanpp
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         )

add_test(NAME "Outputs usage when long help flag is given"
         COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/args003.py ${CMAKE_BINARY_DIR}/pacmanpp
         WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
         )

add_test(NAME "Outputs package name and description when query flag is given"
COMMAND ${Python3_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/args004.py ${CMAKE_BINARY_DIR}/pacmanpp
WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
