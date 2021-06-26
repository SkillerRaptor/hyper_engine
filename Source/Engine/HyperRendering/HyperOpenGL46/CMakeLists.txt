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
# HyperOpenGL46
#-------------------------------------------------------------------------------------------
include(${CMAKE_SOURCE_DIR}/Source/Engine/HyperRendering/HyperCommon/HyperCommon.cmake)

if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(PLATFORM_SOURCES
            src/HyperRendering/OpenGL46/Windows/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            src/HyperRendering/OpenGL46/Windows/PlatformContext.hpp)
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(PLATFORM_SOURCES
            src/HyperRendering/OpenGL46/Linux/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            src/HyperRendering/OpenGL46/Linux/PlatformContext.hpp)
endif ()

set(CORE_SOURCES
        src/HyperRendering/OpenGL46/Context.cpp
        src/HyperRendering/OpenGL46/IPlatformContext.cpp)

set(CORE_HEADERS
        src/HyperRendering/OpenGL46/Context.hpp
        src/HyperRendering/OpenGL46/IPlatformContext.hpp)

set(SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES} ${HYPERCOMMON_SOURCES})
set(HEADERS ${CORE_HEADERS} ${PLATFORM_HEADERS} ${HYPERCOMMON_HEADERS})

foreach (item IN ITEMS ${SOURCES} ${HEADERS})
    get_filename_component(src_path "${item}" PATH)
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}" "" group_path "${src_path}")
    string(REPLACE ".." "\\" group_path "${group_path}")
    source_group("${group_path}" FILES "${item}")
endforeach ()

add_library(HyperOpenGL46 SHARED ${SOURCES} ${HEADERS})
set_target_properties(HyperOpenGL46 PROPERTIES FOLDER HyperModules/HyperRendering)
target_include_directories(HyperOpenGL46 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src ${HYPERCOMMON_INCLUDE})
target_link_libraries(HyperOpenGL46 PRIVATE HyperCore HyperMath HyperPlatform)

find_package(OpenGL)
target_include_directories(HyperOpenGL46 PRIVATE ${OPENGL_INCLUDE_DIRS})
target_link_libraries(HyperOpenGL46 PRIVATE ${OPENGL_LIBRARIES})

add_custom_command(
        TARGET HyperOpenGL46
        POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:HyperOpenGL46> ${CMAKE_BINARY_DIR}/Source/Editor/)
