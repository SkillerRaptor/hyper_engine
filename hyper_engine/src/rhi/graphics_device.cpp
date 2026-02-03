/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "rhi/graphics_device.hpp"

#include "core/assertion.hpp"
#include "rhi/vulkan/graphics_device.hpp"

namespace he
{
    RefPtr<GraphicsDevice> GraphicsDevice::create(
        const GraphicsApi graphics_api, const Window &window, const Validation validation_requested)
    {
        switch (graphics_api)
        {
        case GraphicsApi::Vulkan:
            return VulkanGraphicsDevice::create(window, validation_requested);
        default:
            HE_UNREACHABLE();
        }
    }
} // namespace he
