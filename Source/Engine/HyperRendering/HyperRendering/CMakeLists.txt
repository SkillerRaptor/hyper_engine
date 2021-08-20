#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperRenderingBase
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperRendering/IContext.cpp
        src/HyperRendering/RenderCommands.cpp)

set(HEADERS
        include/HyperRendering/IContext.hpp
        include/HyperRendering/RenderCommands.hpp)

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_module(HyperRendering)
target_link_libraries(
        HyperRendering
        PUBLIC
        HyperCore
        HyperMath
        HyperPlatform)
