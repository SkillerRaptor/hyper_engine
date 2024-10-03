/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/graphics_device.hpp"

#include <hyper_core/assertion.hpp>

#if HE_WINDOWS
#    include "hyper_rhi/d3d12/graphics_device.hpp"
#endif

#include "hyper_rhi/vulkan/graphics_device.hpp"

namespace hyper_rhi
{
    std::unique_ptr<GraphicsDevice> GraphicsDevice::create(const GraphicsDeviceDescriptor &descriptor)
    {
        switch (descriptor.graphics_api)
        {
        case GraphicsApi::D3D12:
#if HE_WINDOWS
            return std::make_unique<D3D12GraphicsDevice>(descriptor);
#else
            return nullptr;
#endif
        case GraphicsApi::Vulkan:
            return std::make_unique<VulkanGraphicsDevice>(descriptor);
        default:
            HE_UNREACHABLE();
        }
    }
} // namespace hyper_rhi
