#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# Tests
#-------------------------------------------------------------------------------------------
if(CONAN_CATCH2_ROOT_DEBUG)
    include(${CONAN_CATCH2_ROOT_DEBUG}/lib/cmake/Catch2/Catch.cmake)
else()
    include(${CONAN_CATCH2_ROOT}/lib/cmake/Catch2/Catch.cmake)
endif()

add_library(HyperTestsMain STATIC src/main.cpp)
target_link_libraries(HyperTestsMain PUBLIC CONAN_PKG::catch2)
target_link_libraries(HyperTestsMain PRIVATE ProjectOptions)

set(HYPERTEST_SOURCE
        src/Test.cpp)

add_executable(HyperTest ${HYPERTEST_SOURCE})
target_link_libraries(HyperTest PRIVATE ProjectWarnings ProjectOptions HyperTestsMain)

catch_discover_tests(
        HyperTest
        TEST_PREFIX "HyperTest."
        REPORTER xml
        OUTPUT_DIR .
        OUTPUT_PREFIX "HyperTest."
        OUTPUT_SUFFIX .xml)

set(HYPERCONSTEXPRTEST_SOURCE
        src/ConstexprTest.cpp)

add_executable(HyperConstexprTest ${HYPERCONSTEXPRTEST_SOURCE})
target_link_libraries(HyperConstexprTest PRIVATE ProjectWarnings ProjectOptions HyperTestsMain)

catch_discover_tests(
        HyperConstexprTest
        TEST_PREFIX "HyperConstexprTest."
        REPORTER xml
        OUTPUT_DIR .
        OUTPUT_PREFIX "HyperConstexprTest."
        OUTPUT_SUFFIX .xml)
