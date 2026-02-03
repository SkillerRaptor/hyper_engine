#-------------------------------------------------------------------------------------------
# Copyright (c) 2026-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

enable_testing()

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

set(SOURCES
        src/tests.cpp)

add_executable(tests ${SOURCES})
target_link_libraries(
        tests
        PRIVATE
        ProjectWarnings
        ProjectOptions
        Catch2::Catch2WithMain)

catch_discover_tests(
        tests
        TEST_PREFIX "unit_tests."
        REPORTER XML
        OUTPUT_DIR .
        OUTPUT_PREFIX "unit_tests."
        OUTPUT_SUFFIX .xml)