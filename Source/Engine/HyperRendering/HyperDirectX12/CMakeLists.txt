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
# HyperDirectX12
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperRendering/DirectX12/Context.cpp)

set(HEADERS
        include/HyperRendering/DirectX12/Context.hpp)

hyperengine_group_source()

hyperengine_define_module(HyperDirectX12)
target_link_libraries(HyperDirectX12 PUBLIC HyperCore)
target_link_libraries(HyperDirectX12 PUBLIC HyperPlatform)
target_link_libraries(HyperDirectX12 PUBLIC HyperRenderingBase)
