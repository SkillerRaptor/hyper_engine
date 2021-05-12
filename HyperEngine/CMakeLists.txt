#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(HyperEngineLib LANGUAGES C CXX)

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
# Working Modules
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperAnimation)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperAudio)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperGame)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperNetwork)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperPhysics)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperRendering)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperResource)

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperEngine)
