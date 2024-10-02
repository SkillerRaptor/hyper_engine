#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

get_filename_component(SOURCE_DIR "${CMAKE_SOURCE_DIR}" REALPATH)
get_filename_component(BINARY_DIR "${CMAKE_BINARY_DIR}" REALPATH)

if ("${SOURCE_DIR}" STREQUAL "${BINARY_DIR}")
    message(STATUS "######################################################")
    message(STATUS "Warning: in-source builds are disabled")
    message(STATUS "Please create a separate build directory and run cmake from there")
    message(STATUS "######################################################")
    message(FATAL_ERROR "Quitting configuration")
endif ()
