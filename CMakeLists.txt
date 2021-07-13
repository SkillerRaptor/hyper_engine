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
# Global Flags
#-------------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)

#-------------------------------------------------------------------------------------------
# Properties
#-------------------------------------------------------------------------------------------
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#-------------------------------------------------------------------------------------------
# Variables
#-------------------------------------------------------------------------------------------
if (WIN32)
    set(HYPERENGINE_ARCH "Windows")
elseif (UNIX AND NOT APPLE)
    set(HYPERENGINE_ARCH "Linux")
else ()
    set(HYPERENGINE_ARCH "Undefined")
endif ()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    set(HYPERENGINE_BUILD_TYPE "Debug")
else ()
    set(HYPERENGINE_BUILD_TYPE "Release")
endif ()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(HYPERENGINE_COMPILER "Clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(HYPERENGINE_COMPILER "GNU")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(HYPERENGINE_COMPILER "MSVC")
else ()
    set(HYPERENGINE_COMPILER "Undefined")
endif ()

#-------------------------------------------------------------------------------------------
# Flags
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_COMPILER}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP8")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_CRT_SECURE_NO_DEPRECATE /D_CRT_SECURE_NO_WARNINGS")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_SCL_SECURE_NO_DEPRECATE /D_SCL_SECURE_NO_WARNINGS")
endif ()

if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Debug")
    if ("${HYPERENGINE_COMPILER}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    elseif ("${HYPERENGINE_COMPILER}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
    elseif ("${HYPERENGINE_COMPILER}" STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /Od /Zi")
    endif ()
elseif ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Release")
    if ("${HYPERENGINE_COMPILER}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -m64 -mavx2 -mfma -ffast-math")
    elseif ("${HYPERENGINE_COMPILER}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -m64 -mavx2 -mfma -ffast-math")
    elseif ("${HYPERENGINE_COMPILER}" STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O2 /arch:AVX2 /fp:fast")
    endif ()
endif ()

#-------------------------------------------------------------------------------------------
# Source
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Source)
