#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# Conan
#-------------------------------------------------------------------------------------------
macro(run_conan)
    if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
        message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
        file(DOWNLOAD "https://github.com/conan-io/cmake-conan/raw/v0.16/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake")
    endif()

    include(${CMAKE_BINARY_DIR}/conan.cmake)

    conan_add_remote(
            NAME
            conan-center
            URL
            https://conan.bintray.com)

    conan_cmake_run(
            REQUIRES
            ${CONAN_LIBRARIES}
            OPTIONS
            ${CONAN_OPTIONS}
            BASIC_SETUP
            CMAKE_TARGETS
            BUILD
            missing)
endmacro()
