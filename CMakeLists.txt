#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)
project(HyperEngine LANGUAGES C CXX)

#-------------------------------------------------------------------------------------------
# Modules
#-------------------------------------------------------------------------------------------
include(Development/CMake/StandardProjectSettings.cmake)
include(Development/CMake/PreventInSourceBuilds.cmake)

#-------------------------------------------------------------------------------------------
# Project Libraries
#-------------------------------------------------------------------------------------------
add_library(ProjectOptions INTERFACE)
target_compile_features(ProjectOptions INTERFACE cxx_std_17)

add_library(ProjectWarnings INTERFACE)

#-------------------------------------------------------------------------------------------
# Standard Settings
#-------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

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
# Conan
#-------------------------------------------------------------------------------------------
set(CONAN_LIBRARIES
        assimp/5.0.1
        catch2/2.13.7
        fmt/8.0.1
        stb/20200203)

include(Development/CMake/Conan.cmake)
run_conan()

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------
option(ENABLE_TESTING "Enable Test Builds" ON)

#-------------------------------------------------------------------------------------------
# Tests
#-------------------------------------------------------------------------------------------
if(ENABLE_TESTING)
    enable_testing()
endif()

#-------------------------------------------------------------------------------------------
# Source
#-------------------------------------------------------------------------------------------
add_subdirectory(Source)
