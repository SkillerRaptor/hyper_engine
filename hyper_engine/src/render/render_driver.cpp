/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "render/render_driver.hpp"

#include "core/prerequisites.hpp"
#include "render/vulkan/vulkan_render_driver.hpp"

std::unique_ptr<RenderDriver> RenderDriver::create(
    const GraphicsApi graphics_api, void *native_window, const u32 width, const u32 height)
{
    switch (graphics_api)
    {
    case GraphicsApi::Vulkan: return VulkanRenderDriver::create(native_window, width, height);
    default: HE_UNREACHABLE();
    }
}
