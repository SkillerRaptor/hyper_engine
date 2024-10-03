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
    update_mappings
    PROPERTIES
    FOLDER "third_party")
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
    cpptrace-lib
    PROPERTIES
    FOLDER "third_party")
set_target_properties(
    libassert-lib
    PROPERTIES
    FOLDER "third_party")

#-------------------------------------------------------------------------------------------
# volk
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
    volk
    GIT_REPOSITORY https://github.com/zeux/volk.git
    GIT_TAG 1.3.295)

set(VOLK_PULL_IN_VULKAN ON CACHE INTERNAL "")
set(VOLK_INSTALL OFF CACHE INTERNAL "")
set(VOLK_HEADERS_ONLY OFF CACHE INTERNAL "")

FetchContent_MakeAvailable(volk)

set_target_properties(
    volk
    PROPERTIES
    FOLDER "third_party")
