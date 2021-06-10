#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.15)

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
#add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Rendering)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperResource)

#-------------------------------------------------------------------------------------------
# Engine
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperEngine)
