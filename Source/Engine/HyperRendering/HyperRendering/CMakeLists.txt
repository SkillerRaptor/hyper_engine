#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperRenderingBase
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperRendering/IContext.cpp)

set(HEADERS
        include/HyperRendering/IContext.hpp)

hyperengine_group_source(${SOURCES})
hyperengine_group_source(${HEADERS})

hyperengine_define_module(HyperRendering)
target_link_libraries(
        HyperRendering
        PUBLIC
        HyperCore
        HyperPlatform)
