#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperVulkan
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperVulkan/GraphicsContext.cpp)

set(HEADERS
        include/HyperVulkan/GraphicsContext.hpp)

hyperengine_group_files("${SOURCES}")
hyperengine_group_files("${HEADERS}")

hyperengine_define_module(HyperVulkan)
target_link_libraries(
        HyperVulkan
        PUBLIC
        CONAN_PKG::glfw
        CONAN_PKG::vulkan-memory-allocator
        CONAN_PKG::volk
        HyperRenderingBase)
