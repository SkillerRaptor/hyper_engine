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
# HyperRenderingBase
#-------------------------------------------------------------------------------------------
set(CORE_SOURCES
        src/HyperRendering/RenderCommand.cpp)

set(CORE_HEADERS
        include/HyperRendering/IContext.hpp
        include/HyperRendering/IRenderer.hpp
        include/HyperRendering/RenderCommand.hpp
        include/HyperRendering/RenderingAPI.hpp)

if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(PLATFORM_SOURCES
            "")

    set(PLATFORM_HEADERS
            include/HyperRendering/Windows/RenderingAPI.hpp)
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(PLATFORM_SOURCES
            "")

    set(PLATFORM_HEADERS
            include/HyperRendering/Linux/RenderingAPI.hpp)
endif ()

set(SOURCES ${CORE_SOURCES} ${PLATFORM_SOURCES})
set(HEADERS ${CORE_HEADERS} ${PLATFORM_HEADERS})

hyperengine_group_source()

hyperengine_define_module(HyperRenderingBase)
target_link_libraries(HyperRenderingBase PUBLIC HyperCore)
target_link_libraries(HyperRenderingBase PUBLIC HyperMath)
target_link_libraries(HyperRenderingBase PUBLIC HyperPlatform)
