#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperEngine
#-------------------------------------------------------------------------------------------
set(SOURCES
        )

set(HEADERS
        )

hyperengine_create_library(HyperEngine)
target_link_libraries(
        HyperEngine
        PUBLIC
        glfw
        vulkan-headers
        volk)
