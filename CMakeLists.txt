#-------------------------------------------------------------------------------------------
# Copyright (C) HyperEngine. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10)
project(HyperEngine)

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
# Source
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Source)
