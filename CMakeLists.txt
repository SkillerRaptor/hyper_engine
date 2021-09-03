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

include(Development/CMake/CompilerSettings.cmake)
enable_settings(ProjectOptions)

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
# Conan
#-------------------------------------------------------------------------------------------
set(CONAN_LIBRARIES
        catch2/2.13.7
        fmt/8.0.1
        glad/0.1.34
        glfw/3.3.4
        glslang/11.5.0
        vulkan-headers/1.2.184
        vulkan-memory-allocator/2.3.0
        volk/1.2.170)

set(CONAN_OPTIONS
        glad:gl_profile=core)

include(Development/CMake/Conan.cmake)
run_conan()

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------
option(ENABLE_TESTING "Enable Test Builds" ON)

#-------------------------------------------------------------------------------------------
# Tests
#-------------------------------------------------------------------------------------------
if (ENABLE_TESTING)
    enable_testing()
endif ()

#-------------------------------------------------------------------------------------------
# Source
#-------------------------------------------------------------------------------------------
add_subdirectory(Source)
