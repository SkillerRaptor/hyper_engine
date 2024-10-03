/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/graphics_device.hpp"
#include "hyper_rhi/vulkan/common.hpp"

namespace hyper_rhi
{
    class VulkanGraphicsDevice : public GraphicsDevice
    {
    public:
        VulkanGraphicsDevice(const GraphicsDeviceDescriptor &descriptor);
        ~VulkanGraphicsDevice();

    private:
        bool check_validation_layer_support();
        void create_instance();
        void create_debug_messenger();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
            void *);

    private:
        bool m_validation_layers_enabled;
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;
    };
} // namespace hyper_rhi
