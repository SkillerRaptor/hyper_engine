/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

// clang-format off
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format on

#include <vector>

#include <hyper_core/types.hpp>

#include "hyper_rhi/graphics_device.hpp"

namespace he
{
    class VulkanGraphicsDevice final : public GraphicsDevice
    {
    public:
        static RefPtr<VulkanGraphicsDevice> create(const Window &, Validation validation_requested);
        ~VulkanGraphicsDevice() override;

        RefPtr<Buffer> create_buffer(const BufferDescriptor &) override;
        RefPtr<Shader> create_shader(const ShaderDescriptor &) override;
        RefPtr<Sampler> create_sampler(const SamplerDescriptor &) override;
        RefPtr<Texture> create_texture(const TextureDescriptor &) override;
        RefPtr<PipelineLayout> create_pipeline_layout(const PipelineLayoutDescriptor &) override;
        RefPtr<ComputePipeline> create_compute_pipeline(const ComputePipelineDescriptor &) override;
        RefPtr<RenderPipeline> create_render_pipeline(const RenderPipelineDescriptor &) override;

        HE_ALWAYS_INLINE VkDevice device() const { return m_device; }
        HE_ALWAYS_INLINE VmaAllocator &allocator() { return m_allocator; }

        HE_ALWAYS_INLINE VkDescriptorSetLayout storage_buffer_layout() const { return m_storage_buffer_layout; };
        HE_ALWAYS_INLINE VkDescriptorSetLayout sampled_image_layout() const { return m_sampled_image_layout; };
        HE_ALWAYS_INLINE VkDescriptorSetLayout storage_image_layout() const { return m_storage_image_layout; };
        HE_ALWAYS_INLINE VkDescriptorSetLayout sampler_layout() const { return m_sampler_layout; };

    private:
        VulkanGraphicsDevice() = default;

        bool create_instance(Validation validation_requested);
        bool create_device();
        bool create_allocator();
        bool create_surface(const Window &);
        bool create_swapchain(const Window &window);
        bool create_descriptors();

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT,
            VkDebugUtilsMessageTypeFlagsEXT,
            const VkDebugUtilsMessengerCallbackDataEXT *,
            void *);

    private:
        VkInstance m_instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;

        u32 m_queue_family = 0;
        u32 m_sample_count = 1;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;
        VkQueue m_queue = VK_NULL_HANDLE;

        VmaAllocator m_allocator = VK_NULL_HANDLE;

        VkSurfaceKHR m_surface = VK_NULL_HANDLE;

        VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
        std::vector<RefPtr<Texture>> m_swapchain_textures;
        // std::vector<RefPtr<TextureView>> m_swapchain_textures_views;
        u32 m_swapchain_texture_index = 0;
        bool m_swapchain_out_of_date = false;

        VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_storage_buffer_layout = VK_NULL_HANDLE;
        VkDescriptorSet m_storage_buffer_set = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_sampled_image_layout = VK_NULL_HANDLE;
        VkDescriptorSet m_sampled_image_set = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_storage_image_layout = VK_NULL_HANDLE;
        VkDescriptorSet m_storage_image_set = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_sampler_layout = VK_NULL_HANDLE;
        VkDescriptorSet m_sampler_set = VK_NULL_HANDLE;
    };
} // namespace he
