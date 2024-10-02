#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

include(FetchContent)

#-------------------------------------------------------------------------------------------
# fmt
#-------------------------------------------------------------------------------------------
FetchContent_Declare(
    fmt
    GIT_REPOSITORY https://github.com/fmtlib/fmt.git
    GIT_TAG 11.0.2
)

set(FMT_DOC OFF)
set(FMT_INSTALL OFF)
set(FMT_TEST OFF)
set(FMT_FUZZ OFF)
set(FMT_CUDA_TEST OFF)
set(FMT_OS ON)
set(FMT_MODULE OFF)
set(FMT_SYSTEM_HEADERS OFF)
set(FMT_UNICODE ON)

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
    GIT_TAG 3.4
)

set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
set(GLFW_INSTALL OFF)

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
    GIT_TAG v2.1.1
)

FetchContent_MakeAvailable(libassert)

target_compile_definitions(libassert-lib PUBLIC LIBASSERT_PREFIX_ASSERTIONS)
