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

    CommandEncoder GraphicsDevice::acquire_command_encoder()
    {
        const u32 frame_id = m_frame_index % s_frames_in_flight;
        return acquire_command_encoder_impl(frame_id);
    }

    void GraphicsDevice::submit_command_encoder(CommandEncoder command_encoder)
    {
        submit_command_encoder_impl(std::move(command_encoder));
        m_frame_index += 1;
    }
} // namespace he
