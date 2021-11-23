#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)
project(HyperEngine LANGUAGES C CXX)

#-------------------------------------------------------------------------------------------
# Modules
#-------------------------------------------------------------------------------------------
include(Development/CMake/StandardProjectSettings.cmake)

#-------------------------------------------------------------------------------------------
# Project Libraries
#-------------------------------------------------------------------------------------------
add_library(ProjectOptions INTERFACE)
target_compile_features(ProjectOptions INTERFACE cxx_std_20)

add_library(ProjectWarnings INTERFACE)

#-------------------------------------------------------------------------------------------
# Standard Settings
#-------------------------------------------------------------------------------------------
include(Development/CMake/Cache.cmake)

include(Development/CMake/CompilerWarnings.cmake)
enable_warnings(ProjectWarnings)

include(Development/CMake/Sanitizers.cmake)
enable_sanitizers(ProjectOptions)

include(Development/CMake/Doxygen.cmake)
enable_doxygen()

include(Development/CMake/StaticAnalyzers.cmake)

include(Development/CMake/Utils.cmake)

#-------------------------------------------------------------------------------------------
# Variables
#-------------------------------------------------------------------------------------------
if (WIN32)
    set(HYPERENGINE_ARCH "Windows")
elseif (UNIX AND NOT APPLE)
    set(HYPERENGINE_ARCH "Linux")
endif ()

#-------------------------------------------------------------------------------------------
# Source
#-------------------------------------------------------------------------------------------
add_subdirectory(Source)
