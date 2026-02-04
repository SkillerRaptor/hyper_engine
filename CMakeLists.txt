#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)
project(
        HyperEngine
        VERSION 0.0.1
        DESCRIPTION ""
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#-------------------------------------------------------------------------------------------
# Global Options
#-------------------------------------------------------------------------------------------
include(cmake/PreventInSourceBuilds.cmake)
include(cmake/Utils.cmake)

include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

option(HE_ENABLE_IPO "Enable IPO/LTO" ON)
option(HE_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
option(HE_ENABLE_CACHE "Enable ccache" OFF)

mark_as_advanced(
        HE_ENABLE_IPO
        HE_WARNINGS_AS_ERRORS
        HE_ENABLE_CACHE)

if (HE_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    he_enable_ipo()
endif ()

#-------------------------------------------------------------------------------------------
# Third Party
#-------------------------------------------------------------------------------------------
add_subdirectory(third_party)

#-------------------------------------------------------------------------------------------
# Local Options
#-------------------------------------------------------------------------------------------
include(cmake/StandardProjectSettings.cmake)

add_library(ProjectWarnings INTERFACE)

include(cmake/CompilerWarnings.cmake)
he_set_project_warnings(ProjectWarnings ${HE_WARNINGS_AS_ERRORS})

add_library(ProjectOptions INTERFACE)
target_compile_features(ProjectOptions INTERFACE cxx_std_${CMAKE_CXX_STANDARD})

if (HE_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    he_enable_cache()
endif ()

set(CMAKE_CXX_VISIBILITY_PRESET hidden)

#-------------------------------------------------------------------------------------------
# Source
#-------------------------------------------------------------------------------------------
add_subdirectory(hyper_core)
add_subdirectory(hyper_platform)
add_subdirectory(hyper_rhi)
add_subdirectory(hyper_engine)

set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT hyper_engine)

#-------------------------------------------------------------------------------------------
# Tests
#-------------------------------------------------------------------------------------------
include(CTest)
if (BUILD_TESTING)
    add_subdirectory(tests)
endif ()