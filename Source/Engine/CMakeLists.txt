#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake Info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)

#-------------------------------------------------------------------------------------------
# Core Module
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperCore)

#-------------------------------------------------------------------------------------------
# Support Modules
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperPlatform)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperMath)

#-------------------------------------------------------------------------------------------
# Abstraction Modules
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperAudio)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperNetwork)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperPhysics)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperRendering)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperResource)

#-------------------------------------------------------------------------------------------
# Engine
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperEngine)
