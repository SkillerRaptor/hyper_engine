/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include <hyper_core/types.hpp>

#include "hyper_rhi/command_encoder.hpp"

namespace he
{
    class VulkanGraphicsDevice;

    class VulkanCommandEncoder : public BackendCommandEncoder
    {
    public:
        explicit VulkanCommandEncoder(VulkanGraphicsDevice &);
        ~VulkanCommandEncoder() override;

        void acquire() override;
        void submit() override;

    private:
        VulkanGraphicsDevice &m_graphics_device;

        VkCommandPool m_command_pool = VK_NULL_HANDLE;
        VkCommandBuffer m_command_buffer = VK_NULL_HANDLE;
        VkFence m_fence = VK_NULL_HANDLE;
        VkSemaphore m_submit_semaphore = VK_NULL_HANDLE;
        u64 m_semaphore_counter = 0;
        bool m_swapchain_texture_acquired = false;
    };
} // namespace he
