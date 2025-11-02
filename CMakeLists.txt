#-------------------------------------------------------------------------------------------
# Copyright (c) 2024-present, SkillerRaptor
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1...4.1)
project(HyperEngine LANGUAGES C CXX)

#-------------------------------------------------------------------------------------------
# Standard Settings
#-------------------------------------------------------------------------------------------
include(cmake/StandardProjectSettings.cmake)

#-------------------------------------------------------------------------------------------
# Project Libraries
#-------------------------------------------------------------------------------------------
add_library(ProjectOptions INTERFACE)
target_compile_features(ProjectOptions INTERFACE cxx_std_23)

add_library(ProjectWarnings INTERFACE)

#-------------------------------------------------------------------------------------------
# Warnings
#-------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 23)
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

add_subdirectory(hyper_engine)
