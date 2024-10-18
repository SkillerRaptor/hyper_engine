/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>

#include "hyper_rhi/graphics_device.hpp"
#include "hyper_rhi/vulkan/common.hpp"

namespace hyper_rhi
{
    class VulkanGraphicsDevice final : public GraphicsDevice
    {
    public:
        explicit VulkanGraphicsDevice(const GraphicsDeviceDescriptor &descriptor);
        ~VulkanGraphicsDevice() override;

        VkInstance instance() const;
        VkPhysicalDevice physical_device() const;
        VkDevice device() const;

    protected:
        SurfaceHandle create_surface(const SurfaceDescriptor &descriptor) override;

        BufferHandle create_buffer(const BufferDescriptor &descriptor) override;
        CommandListHandle create_command_list() override;
        ComputePipelineHandle create_compute_pipeline(const ComputePipelineDescriptor &descriptor) override;
        GraphicsPipelineHandle create_graphics_pipeline(const GraphicsPipelineDescriptor &descriptor) override;
        PipelineLayoutHandle create_pipeline_layout(const PipelineLayoutDescriptor &descriptor) override;
        ShaderModuleHandle create_shader_module(const ShaderModuleDescriptor &descriptor) override;
        TextureHandle create_texture(const TextureDescriptor &descriptor) override;

    private:
        static bool check_validation_layer_support();
        void create_instance();
        void create_debug_messenger();

        void choose_physical_device();
        uint32_t rate_physical_device(const VkPhysicalDevice &physical_device) const;
        std::optional<uint32_t> find_queue_family(const VkPhysicalDevice &physical_device) const;
        static bool check_extension_support(const VkPhysicalDevice &physical_device);
        static bool check_feature_support(const VkPhysicalDevice &physical_device);

        void create_device();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
            void *);

    private:
        bool m_validation_layers_enabled;
        VkInstance m_instance;
        VkDebugUtilsMessengerEXT m_debug_messenger;
        VkPhysicalDevice m_physical_device;
        VkDevice m_device;
        VkQueue m_queue;
    };
} // namespace hyper_rhi
