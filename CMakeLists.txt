#-------------------------------------------------------------------------------------------
# Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)
project(HyperEngine)

#-------------------------------------------------------------------------------------------
# CMake Variables
#-------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#-------------------------------------------------------------------------------------------
# Variables
#-------------------------------------------------------------------------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if (WIN32)
    set(HYPERENGINE_ARCH Windows)
elseif (UNIX AND NOT APPLE)
    set(HYPERENGINE_ARCH Linux)
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(HYPERENGINE_BUILD_TYPE Debug)
else ()
    set(HYPERENGINE_BUILD_TYPE Release)
endif ()

#-------------------------------------------------------------------------------------------
# Standard Flags
#-------------------------------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra ")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP8")
    add_link_options(/DEBUGTYPE:CV,FIXUP,PDATA /INCREMENTAL:NO)
    add_definitions(_CRT_SECURE_NO_DEPRECATE)
    add_definitions(_CRT_SECURE_NO_WARNINGS)
    add_definitions(_SCL_SECURE_NO_DEPRECATE)
    add_definitions(_SCL_SECURE_NO_WARNINGS)
endif ()

#-------------------------------------------------------------------------------------------
# Optimization Flags
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Release")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif ()
endif ()

#-------------------------------------------------------------------------------------------
# Source
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Source)
