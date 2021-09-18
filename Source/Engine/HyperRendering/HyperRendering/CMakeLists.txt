#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperRendering
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperRendering/RenderEngine.cpp)

set(HEADERS
        include/HyperRendering/RenderEngine.hpp)

hyperengine_create_module(HyperRendering)
target_link_libraries(
        HyperRendering
        PUBLIC
        HyperCore
        HyperMath)

#-------------------------------------------------------------------------------------------
# HyperOpenGL
#-------------------------------------------------------------------------------------------
if (HYPERENGINE_BUILD_OPENGL)
    target_link_libraries(
            HyperRendering
            PUBLIC
            HyperOpenGL)
    target_compile_definitions(
            HyperRendering
            PUBLIC
            -DHYPERENGINE_BUILD_OPENGL=1)
endif ()

#-------------------------------------------------------------------------------------------
# HyperVulkan
#-------------------------------------------------------------------------------------------
if (HYPERENGINE_BUILD_VULKAN)
    target_link_libraries(
            HyperRendering
            PUBLIC
            HyperVulkan)
    target_compile_definitions(
            HyperRendering
            PUBLIC
            -DHYPERENGINE_BUILD_VULKAN=1)
endif ()
