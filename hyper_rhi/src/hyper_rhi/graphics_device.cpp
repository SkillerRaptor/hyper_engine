/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/graphics_device.hpp"

#include <hyper_core/assertion.hpp>

#include "hyper_rhi/vulkan/graphics_device.hpp"

namespace he
{
    OwnPtr<GraphicsDevice> GraphicsDevice::create(
        const GraphicsApi graphics_api, const Window &window, const Validation validation_requested)
    {
        switch (graphics_api)
        {
        case GraphicsApi::Vulkan:
            return make_own<VulkanGraphicsDevice>(window, validation_requested);
        default:
            HE_UNREACHABLE();
        }
    }
} // namespace he
