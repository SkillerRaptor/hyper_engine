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
# HyperCommon
#-------------------------------------------------------------------------------------------
set(HYPERCOMMON_PATH ${CMAKE_SOURCE_DIR}/Source/Engine/HyperRendering/HyperCommon)

if ("${HYPERENGINE_ARCH}" STREQUAL "Windows")
    set(HYPERCOMMON_PLATFORM_SOURCES
            "")

    set(HYPERCOMMON_PLATFORM_HEADERS
            ${HYPERCOMMON_PATH}/src/HyperRendering/Windows/RenderingAPI.hpp)
elseif ("${HYPERENGINE_ARCH}" STREQUAL "Linux")
    set(HYPERCOMMON_PLATFORM_SOURCES
            "")

    set(HYPERCOMMON_PLATFORM_HEADERS
            ${HYPERCOMMON_PATH}/src/HyperRendering/Linux/RenderingAPI.hpp)
endif ()

set(HYPERCOMMON_SOURCES
        ${HYPERCOMMON_PATH}/src/HyperRendering/RenderCommand.cpp
        ${HYPERCOMMON_PLATFORM_SOURCES})

set(HYPERCOMMON_HEADERS
        ${HYPERCOMMON_PATH}/src/HyperRendering/IContext.hpp
        ${HYPERCOMMON_PATH}/src/HyperRendering/IRenderer.hpp
        ${HYPERCOMMON_PATH}/src/HyperRendering/RenderCommand.hpp
        ${HYPERCOMMON_PATH}/src/HyperRendering/RenderingAPI.hpp
        ${HYPERCOMMON_PLATFORM_HEADERS})

set(HYPERCOMMON_INCLUDE ${HYPERCOMMON_PATH}/src)
