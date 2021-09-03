#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperResource
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperResource/HyperResource.cpp)

set(HEADERS
        )

hyperengine_define_module(HyperResource)
target_link_libraries(
        HyperResource
        PUBLIC
        HyperCore)
