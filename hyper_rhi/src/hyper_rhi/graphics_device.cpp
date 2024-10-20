/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/graphics_device.hpp"

#include <hyper_core/assertion.hpp>

#if HE_WINDOWS
#    include "hyper_rhi/d3d12/d3d12_graphics_device.hpp"
#endif

#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"

namespace hyper_rhi
{
    std::shared_ptr<GraphicsDevice> GraphicsDevice::create(const GraphicsDeviceDescriptor &descriptor)
    {
        switch (descriptor.graphics_api)
        {
        case GraphicsApi::D3D12:
#if HE_WINDOWS
            return std::make_shared<D3D12GraphicsDevice>(descriptor);
#else
            return nullptr;
#endif
        case GraphicsApi::Vulkan:
            return std::make_shared<VulkanGraphicsDevice>(descriptor);
        default:
            HE_UNREACHABLE();
        }
    }
} // namespace hyper_rhi
