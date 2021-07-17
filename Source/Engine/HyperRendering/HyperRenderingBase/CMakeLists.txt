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
set(SOURCES
        src/HyperRendering/HyperRenderingBase.cpp)

set(HEADERS
        include/HyperRendering/IRenderer.hpp
        include/HyperRendering/IRenderer2D.hpp
        include/HyperRendering/IContext.hpp)

hyperengine_group_source()

hyperengine_define_module(HyperRenderingBase)
target_link_libraries(HyperRenderingBase PUBLIC HyperCore)
target_link_libraries(HyperRenderingBase PUBLIC HyperMath)
