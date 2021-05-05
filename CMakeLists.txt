#-------------------------------------------------------------------------------------------
# Copyright (C) Hyperion. All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(HyperEngine LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/Utils")
include("${CMAKE_CURRENT_SOURCE_DIR}/Utils/HyperPlatform.cmake")
include("${CMAKE_CURRENT_SOURCE_DIR}/Utils/HyperUtils.cmake")

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------
option(HYPERENGINE_BUILD_SAMPLES "Enable generation of build files for samples" ON)
option(HYPERENGINE_BUILD_TESTS "Enable generation of build files for tests" ON)

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    hyperengine_set_cache_value(HYPERENGINE_BUILD_TYPE "Debug")
else ()
    hyperengine_set_cache_value(HYPERENGINE_BUILD_TYPE "Release")
endif ()

#-------------------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/vendor)

#-------------------------------------------------------------------------------------------
# Optimization Flags
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_BUILD_TYPE}" STREQUAL "Release")
    if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g3 -O0 -Wall -pedantic -Wextra -m64 -mavx2 -mfma -ffast-math")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra -m64 -mavx2 -mfma -ffast-math")
    elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
        string(REGEX REPLACE "/W3" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17 /W4 /arch:AVX2 /fp:fast /MP /DNOMINMAX")
    endif ()
endif ()

#-------------------------------------------------------------------------------------------
# Disable Exceptions & RTTI
#-------------------------------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    add_compile_options(-fno-rtti -fno-exceptions)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    add_compile_options(-fno-rtti -fno-exceptions)
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    add_compile_options(/GR- /EHsc)
endif ()

#-------------------------------------------------------------------------------------------
# Projects
#-------------------------------------------------------------------------------------------
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperEngine)

#-----------------------------------------------------------------------------------
# TODO: Updating HyperEditor
#-----------------------------------------------------------------------------------
# add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/HyperEditor)

#-------------------------------------------------------------------------------------------
# Enable Exceptions & RTTI
#-------------------------------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    string(REGEX REPLACE "-fno-rtti -fno-exceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    string(REGEX REPLACE "-fno-rtti -fno-exceptions" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    string(REGEX REPLACE "/GR-" "/GR" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    string(REGEX REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
endif ()

#-------------------------------------------------------------------------------------------
# Subprojects
#-------------------------------------------------------------------------------------------
if (HYPERENGINE_BUILD_SAMPLES)
    #-----------------------------------------------------------------------------------
    # TODO: Updating Samples
    #-----------------------------------------------------------------------------------
    # add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/Samples)
endif ()

if (HYPERENGINE_BUILD_TESTS)
    #-----------------------------------------------------------------------------------
    # TODO: Add tests
    #-----------------------------------------------------------------------------------
endif ()

#-------------------------------------------------------------------------------------------
# Visual Studio
#-------------------------------------------------------------------------------------------
set_property(DIRECTORY PROPERTY VS_STARTUP_PROJECT HyperEditor)