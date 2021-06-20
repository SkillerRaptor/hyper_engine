#-------------------------------------------------------------------------------------------
# Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

#-------------------------------------------------------------------------------------------
# ThirdParty
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/ThirdParty)

#-------------------------------------------------------------------------------------------
# Flags
#-------------------------------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DNOMINMAX /DWIN32_LEAN_AND_MEAN")
endif ()

if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Release")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -mavx2 -mfma -ffast-math")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -m64 -mavx2 -mfma -ffast-math")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2 /fp:fast")
    endif ()
endif ()


#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Engine)

#-------------------------------------------------------------------------------------------
# HyperEditor
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Editor)
