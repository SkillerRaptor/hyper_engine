#-------------------------------------------------------------------------------------------
# Copyright (c) 2024, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)
project(HyperEngine LANGUAGES C CXX)

#-------------------------------------------------------------------------------------------
# Standard Settings
#-------------------------------------------------------------------------------------------
include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

#-------------------------------------------------------------------------------------------
# Project Libraries
#-------------------------------------------------------------------------------------------
add_library(ProjectOptions INTERFACE)
target_compile_features(ProjectOptions INTERFACE cxx_std_20)

add_library(ProjectWarnings INTERFACE)

#-------------------------------------------------------------------------------------------
# Warnings
#-------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(cmake/CompilerWarnings.cmake)
enable_warnings(ProjectWarnings)

include(cmake/Utils.cmake)

#-------------------------------------------------------------------------------------------
# Source
#-------------------------------------------------------------------------------------------
add_subdirectory(third_party)

add_subdirectory(hyper_core)
add_subdirectory(hyper_event)
add_subdirectory(hyper_platform)
add_subdirectory(hyper_rhi)
add_subdirectory(hyper_render)

add_subdirectory(hyper_engine)
