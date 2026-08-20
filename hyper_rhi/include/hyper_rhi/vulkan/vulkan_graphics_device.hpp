/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

// clang-format off
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format on

#include <vector>

#include <hyper_core/types.hpp>

#include "hyper_rhi/graphics_device.hpp"

namespace he {

class VulkanGraphicsDevice : public GraphicsDevice {
private:
    static constexpr const char *s_validation_layer = "VK_LAYER_KHRONOS_validation";
    static constexpr const char *s_validation_extension = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
    static constexpr const char *s_swapchain_extension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

public:
    explicit VulkanGraphicsDevice(const Window &);
    ~VulkanGraphicsDevice() override;

protected:
    Buffer *create_buffer_impl(const BufferDescriptor &) override;
    void destroy_buffer_impl(Buffer *) override;

    Shader *create_shader_impl(const ShaderDescriptor &) override;
    void destroy_shader_impl(Shader *) override;

    Sampler *create_sampler_impl(const SamplerDescriptor &) override;
    void destroy_sampler_impl(Sampler *) override;

    Texture *create_texture_impl(const TextureDescriptor &) override;
    void destroy_texture_impl(Texture *) override;

    TextureView *create_texture_view_impl(const TextureViewDescriptor &) override;
    void destroy_texture_view_impl(TextureView *) override;

    PipelineLayout *create_pipeline_layout_impl(const PipelineLayoutDescriptor &) override;
    void destroy_pipeline_layout_impl(PipelineLayout *) override;

    ComputePipeline *create_compute_pipeline_impl(const ComputePipelineDescriptor &) override;
    void destroy_compute_pipeline_impl(ComputePipeline *) override;

    RenderPipeline *create_render_pipeline_impl(const RenderPipelineDescriptor &) override;
    void destroy_render_pipeline_impl(RenderPipeline *) override;

private:
    void create_instance();
    static bool check_validation_layer_support();

    void choose_physical_device();
    void create_logical_device();

    void create_allocator();

    void create_surface(const Window &);
    void create_swapchain(const Window &);
    VkExtent2D choose_extent(const Window &) const;
    VkSurfaceFormatKHR choose_surface_format() const;
    VkPresentModeKHR choose_present_mode() const;

    void create_descriptors();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT,
        VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT *,
        void *);

private:
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;

    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    u32 m_queue_family = 0;
    VkDevice m_device = VK_NULL_HANDLE;
    VkQueue m_queue = VK_NULL_HANDLE;

    VmaAllocator m_allocator = VK_NULL_HANDLE;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    std::vector<VkImage> m_swapchain_images = { };
    std::vector<VkImageView> m_swapchain_image_views = { };

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
