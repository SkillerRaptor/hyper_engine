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
# HyperDirectX11
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperRendering/DirectX11/Context.cpp)

set(HEADERS
        include/HyperRendering/DirectX11/Context.hpp)

group_source()

define_module(HyperDirectX11)
target_link_libraries(HyperDirectX11 PUBLIC HyperCore)
target_link_libraries(HyperDirectX11 PUBLIC HyperPlatform)
target_link_libraries(HyperDirectX11 PUBLIC HyperRenderingBase)
