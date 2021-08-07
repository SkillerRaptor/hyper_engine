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

set(HYPERMATH_SOURCE
        src/HyperMath/Common.cpp
        src/HyperMath/Constants.cpp
        src/HyperMath/Trigonometric.cpp)

add_executable(HyperMathTest ${HYPERMATH_SOURCE})
target_link_libraries(
        HyperMathTest
        PRIVATE
        ProjectWarnings
        ProjectOptions
        HyperMath
        HyperTestMain)

catch_discover_tests(
        HyperMathTest
        TEST_PREFIX "HyperMathTest."
        REPORTER xml
        OUTPUT_DIR .
        OUTPUT_PREFIX "HyperMathTest."
        OUTPUT_SUFFIX .xml)
