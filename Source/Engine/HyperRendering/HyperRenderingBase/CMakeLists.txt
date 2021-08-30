#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperRenderingBase
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperRenderingBase/IGraphicsContext.cpp)

set(HEADERS
        include/HyperRenderingBase/IGraphicsContext.hpp
        include/HyperRenderingBase/Resources.hpp)

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_module(HyperRenderingBase)
target_link_libraries(
        HyperRenderingBase
        PUBLIC
        HyperCore
        HyperGame
        HyperPlatform)
