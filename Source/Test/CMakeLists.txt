#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# Tests
#-------------------------------------------------------------------------------------------
if (CONAN_CATCH2_ROOT_DEBUG)
    include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else ()
    include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif ()

set(SOURCES
        src/main.cpp)

hyperengine_define_library(HyperTestMain)
hyperengine_set_folder(HyperTestMain HyperTests)
target_link_libraries(
        HyperTestMain
        PUBLIC
        CONAN_PKG::catch2)

macro(define_test target source)
    set(SOURCES "${source}")

    hyperengine_define_executable(${target})
    hyperengine_set_folder(${target} HyperTests)
    target_link_libraries(
            ${target}
            PRIVATE
            HyperTestMain)

    hyperengine_move_target(${target} Tests)

    catch_discover_tests(
            ${target}
            TEST_PREFIX "${target}."
            REPORTER xml
            OUTPUT_DIR .
            OUTPUT_PREFIX "${target}."
            OUTPUT_SUFFIX .xml)
endmacro()

set(HYPERMATH_SOURCE
        src/HyperMath/Common.cpp
        src/HyperMath/Trigonometric.cpp)

define_test(HyperMathTest "${HYPERMATH_SOURCE}")
target_link_libraries(
        HyperMathTest
        PRIVATE
        HyperMath)
