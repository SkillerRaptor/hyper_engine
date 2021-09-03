#-------------------------------------------------------------------------------------------
# Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
#
# SPDX-License-Identifier: MIT
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# HyperVulkan
#-------------------------------------------------------------------------------------------
set(SOURCES
        src/HyperVulkan/Device.cpp
        src/HyperVulkan/GraphicsContext.cpp
        src/HyperVulkan/GraphicsPipeline.cpp
        src/HyperVulkan/SwapChain.cpp)

set(HEADERS
        include/HyperVulkan/Device.hpp
        include/HyperVulkan/GraphicsContext.hpp
        include/HyperVulkan/GraphicsPipeline.hpp
        include/HyperVulkan/SwapChain.hpp)

hyperengine_define_module(HyperVulkan)
target_link_libraries(
        HyperVulkan
        PUBLIC
        CONAN_PKG::glfw
        CONAN_PKG::vulkan-memory-allocator
        CONAN_PKG::volk
        HyperRenderingBase
        HyperResource)
