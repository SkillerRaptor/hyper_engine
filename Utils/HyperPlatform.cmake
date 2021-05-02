#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

include("${CMAKE_SOURCE_DIR}/Utils/HyperUtils.cmake")

#-------------------------------------------------------------------------------------------
# Platform
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(HYPERENGINE_WINDOWS FALSE)
hyperengine_set_cache_value(HYPERENGINE_APPLE FALSE)
hyperengine_set_cache_value(HYPERENGINE_LINUX FALSE)

if (WIN32)
    hyperengine_set_cache_value(HYPERENGINE_WINDOWS TRUE)
endif ()

if (APPLE)
    hyperengine_set_cache_value(HYPERENGINE_APPLE TRUE)
endif ()

if (UNIX AND NOT APPLE)
    hyperengine_set_cache_value(HYPERENGINE_LINUX TRUE)
endif ()

#-------------------------------------------------------------------------------------------
# API
#-------------------------------------------------------------------------------------------
hyperengine_set_cache_value(HYPERENGINE_DIRECTX FALSE)
hyperengine_set_cache_value(HYPERENGINE_VULKAN FALSE)

if (HYPERENGINE_WINDOWS)
    hyperengine_set_cache_value(HYPERENGINE_DIRECTX TRUE)
endif ()

find_package(Vulkan)
if(Vulkan_FOUND)
    hyperengine_set_cache_value(HYPERENGINE_VULKAN TRUE)
endif()