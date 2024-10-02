#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

include(FetchContent)

#-------------------------------------------------------------------------------------------
# argparse
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
    argparse
    GIT_REPOSITORY https://github.com/p-ranav/argparse.git
    GIT_TAG v3.1)

set(ARGPARSE_INSTALL OFF CACHE INTERNAL "")
set(ARGPARSE_BUILD_TESTS OFF CACHE INTERNAL "")
set(ARGPARSE_BUILD_SAMPLES OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(argparse)

set_target_properties(
    argparse
    PROPERTIES
    FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.0.2)

set(FMT_DOC OFF CACHE INTERNAL "")
set(FMT_INSTALL OFF CACHE INTERNAL "")
set(FMT_TEST OFF CACHE INTERNAL "")
set(FMT_FUZZ OFF CACHE INTERNAL "")
set(FMT_CUDA_TEST OFF CACHE INTERNAL "")
set(FMT_OS ON CACHE INTERNAL "")
set(FMT_MODULE OFF CACHE INTERNAL "")
set(FMT_SYSTEM_HEADERS OFF CACHE INTERNAL "")
set(FMT_UNICODE ON CACHE INTERNAL "")

FetchContent_MakeAvailable(fmt)

set_target_properties(
    fmt
    PROPERTIES
    FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# glfw
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4)

set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "")
set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "")
set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "")
set(GLFW_INSTALL OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(glfw)

set_target_properties(
    glfw
    PROPERTIES
    FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# libassert
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
    libassert
    GIT_REPOSITORY https://github.com/jeremy-rifkin/libassert.git
    GIT_TAG v2.1.1)

FetchContent_MakeAvailable(libassert)

target_compile_definitions(libassert-lib PUBLIC LIBASSERT_PREFIX_ASSERTIONS)

set_target_properties(
    libassert-lib
    PROPERTIES
    FOLDER "third_party")
