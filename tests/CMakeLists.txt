#-------------------------------------------------------------------------------------------
# Copyright (c) 2026-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

enable_testing()

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

set(SOURCES
        src/bitflags_tests.cpp
        src/key_tests.cpp
        src/macro_utils_tests.cpp
        src/string_utils_tests.cpp)

add_executable(tests ${SOURCES})
target_link_libraries(
        tests
        PRIVATE
        ProjectWarnings
        ProjectOptions
        Catch2::Catch2WithMain
        hyper_core
        hyper_platform
        hyper_rhi)

catch_discover_tests(
        tests
        REPORTER XML
        OUTPUT_DIR .
        OUTPUT_SUFFIX .xml)