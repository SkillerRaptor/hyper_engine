#-------------------------------------------------------------------------------------------
# Copyright (c) 2026-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

enable_testing()

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

set(HEADERS
        include/dynamic_test_invoker.hpp)

set(SOURCES
        src/hyper_script/integration/pipeline_tests.cpp)

add_executable(tests ${SOURCES} ${HEADERS})
target_link_libraries(
        tests
        PRIVATE
        ProjectWarnings
        ProjectOptions
        Catch2::Catch2WithMain
        hyper_core
        hyper_platform
        hyper_rhi
        hyper_script)
target_include_directories(tests PRIVATE include)
target_compile_options(tests PRIVATE "$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>")

add_custom_command(
        TARGET tests
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/test_data
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_BINARY_DIR}/test_data)

catch_discover_tests(
        tests
        REPORTER XML
        OUTPUT_DIR .
        OUTPUT_SUFFIX .xml)