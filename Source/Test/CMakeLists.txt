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

add_library(HyperTestMain STATIC src/main.cpp)
target_link_libraries(HyperTestMain PUBLIC CONAN_PKG::catch2)
target_link_libraries(HyperTestMain PRIVATE ProjectOptions)

macro(define_test target sources)
    add_executable(${target} ${sources})
    target_link_libraries(
            ${target}
            PRIVATE
            ProjectWarnings
            ProjectOptions
            HyperTestMain)

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

define_test(HyperMathTest ${HYPERMATH_SOURCE})
target_link_libraries(
        HyperMathTest
        PRIVATE
        HyperMath)
