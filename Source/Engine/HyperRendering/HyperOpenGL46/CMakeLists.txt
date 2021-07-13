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
# HyperOpenGL46
#-------------------------------------------------------------------------------------------
if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(PLATFORM_SOURCES
            src/HyperRendering/OpenGL46/Windows/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            include/HyperRendering/OpenGL46/Windows/PlatformContext.hpp)
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(PLATFORM_SOURCES
            src/HyperRendering/OpenGL46/Linux/PlatformContext.cpp)

    set(PLATFORM_HEADERS
            include/HyperRendering/OpenGL46/Linux/PlatformContext.hpp)
endif ()

set(CORE_SOURCES
        src/HyperRendering/OpenGL46/Context.cpp
        src/HyperRendering/OpenGL46/IPlatformContext.cpp)

set(CORE_HEADERS
        include/HyperRendering/OpenGL46/Context.hpp
        include/HyperRendering/OpenGL46/IPlatformContext.hpp)

set(SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES} ${HYPERCOMMON_SOURCES})
set(HEADERS ${CORE_HEADERS} ${PLATFORM_HEADERS} ${HYPERCOMMON_HEADERS})

hyperengine_group_source()

hyperengine_define_module(HyperOpenGL46)
target_include_directories(HyperOpenGL46 PRIVATE ${OPENGL_INCLUDE_DIRS})
target_link_libraries(HyperOpenGL46 PUBLIC HyperCore)
target_link_libraries(HyperOpenGL46 PUBLIC HyperPlatform)
target_link_libraries(HyperOpenGL46 PUBLIC HyperRenderingBase)
target_link_libraries(HyperOpenGL46 PRIVATE ${OPENGL_LIBRARIES})
