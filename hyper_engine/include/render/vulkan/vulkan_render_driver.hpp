/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <unordered_set>
#include <utility>
#include <vector>

// clang-format off
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format on

#include "render/render_driver.hpp"
#include "render/shader_compiler.hpp"

class VulkanRenderDriver final : public RenderDriver
{
private:
    static constexpr const char *s_validation_layer { "VK_LAYER_KHRONOS_validation" };

    enum class DescriptorType : u8
    {
        StorageBuffer = 0,
        SampledImage,
        StorageImage,
        Sampler,
        _Count,
    };

    static constexpr u8 s_descriptor_count = static_cast<u8>(DescriptorType::_Count);
    static constexpr u32 s_descriptor_limit { 1000 * 1000 };

private:
    struct Instance
    {
        VkInstance raw { VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT debug_messenger { VK_NULL_HANDLE };
    };

    struct PhysicalDevice
    {
        struct FeatureSet
        {
            VkPhysicalDeviceFeatures2 features {};
            VkPhysicalDeviceVulkan12Features features_12 {};
            VkPhysicalDeviceVulkan13Features features_13 {};
        };

        std::unordered_set<std::string> extensions {};
        FeatureSet feature_set;
        u32 queue_family { 0 };
        VkPhysicalDevice raw {};
    };

    struct Device
    {
        u32 queue_family { 0 };
        VkPhysicalDevice physical_device { VK_NULL_HANDLE };
        VkDevice raw { VK_NULL_HANDLE };
        VkQueue queue { VK_NULL_HANDLE };
    };

    struct Swapchain
    {
        u32 width { 0 };
        u32 height { 0 };
        VkSwapchainKHR raw { VK_NULL_HANDLE };
        std::vector<TextureId> textures {};
        bool out_of_date { false };
        u32 texture_index { 0 };
    };

    struct Descriptors
    {
        VkDescriptorPool descriptor_pool { VK_NULL_HANDLE };
        std::array<VkDescriptorSetLayout, s_descriptor_count> descriptor_set_layouts {};
        std::array<VkDescriptorSet, s_descriptor_count> descriptor_sets {};
    };

    struct VulkanBuffer : public Buffer
    {
        VkBuffer raw { VK_NULL_HANDLE };
        VmaAllocation allocation { VK_NULL_HANDLE };
    };

    struct VulkanShader : public Shader
    {
        VkShaderModule raw { VK_NULL_HANDLE };
    };

    struct VulkanSampler : public Sampler
    {
        VkSampler raw { VK_NULL_HANDLE };
    };

    struct VulkanTexture : public Texture
    {
        VkImage raw { VK_NULL_HANDLE };
        VmaAllocation allocation { VK_NULL_HANDLE };
        VkImageLayout layout { VK_IMAGE_LAYOUT_UNDEFINED };
    };

    struct VulkanTextureView : public TextureView
    {
        VkImageView raw { VK_NULL_HANDLE };
    };

    struct VulkanPipelineLayout : public PipelineLayout
    {
        VkPipelineLayout raw { VK_NULL_HANDLE };
    };

    struct VulkanComputePipeline : public ComputePipeline
    {
        VkPipeline raw { VK_NULL_HANDLE };
    };

    struct VulkanRenderPipeline : public RenderPipeline
    {
        VkPipeline raw { VK_NULL_HANDLE };
    };

    struct VulkanCommandBuffer : public CommandBuffer
    {
        VkCommandPool command_pool { VK_NULL_HANDLE };
        VkCommandBuffer raw { VK_NULL_HANDLE };
        VkFence render_fence { VK_NULL_HANDLE };
        VkSemaphore submit_semaphore { VK_NULL_HANDLE };
        uint64_t semaphore_counter { 0 };
        bool swapchain_texture_acquired { false };
    };

public:
    static std::unique_ptr<VulkanRenderDriver> create(void *native_window, u32 width, u32 height);
    ~VulkanRenderDriver() override;

    void wait_idle() const override;

    BufferId create_buffer(const BufferDescriptor &) override;
    void destroy_buffer(BufferId) override;

    void *map_buffer(BufferId) const override;
    void unmap_buffer(BufferId) const override;

    void bind_buffer_to_slot(BufferId, ResourceHandle) const override;

    ShaderId create_shader(const ShaderDescriptor &) override;
    void destroy_shader(ShaderId) override;

    SamplerId create_sampler(const SamplerDescriptor &) override;
    void destroy_sampler(SamplerId) override;

    void bind_sampler_to_slot(SamplerId, ResourceHandle) const override;

    TextureId create_texture(const TextureDescriptor &) override;
    void destroy_texture(TextureId) override;

    void generate_mip_maps(CommandBufferId, TextureId) override;
    void transition_to_general(CommandBufferId, TextureId) override;

    TextureViewId create_texture_view(const TextureViewDescriptor &) override;
    void destroy_texture_view(TextureViewId) override;

    void bind_texture_view_to_slot(TextureViewId, ResourceHandle) const override;

    PipelineLayoutId create_pipeline_layout(const PipelineLayoutDescriptor &) override;
    void destroy_pipeline_layout(PipelineLayoutId) override;

    ComputePipelineId create_compute_pipeline(const ComputePipelineDescriptor &) override;
    void destroy_compute_pipeline(ComputePipelineId) override;

    RenderPipelineId create_render_pipeline(const RenderPipelineDescriptor &) override;
    void destroy_render_pipeline(RenderPipelineId) override;

    CommandBufferId create_command_buffer() override;
    void destroy_command_buffer(CommandBufferId) override;

    void acquire_command_buffer(CommandBufferId) override;
    void submit_command_buffer(CommandBufferId) override;

    void begin_gpu_marker(CommandBufferId, PassLabel) const override;
    void end_gpu_marker(CommandBufferId) const override;
    void insert_gpu_marker(CommandBufferId, PassLabel) const override;

    std::pair<TextureViewId, bool> acquire_swapchain_texture(CommandBufferId) override;
    void present() override;
    std::vector<TextureViewId> query_swapchain_texture_views() const override;
    void on_resize(const WindowResizeEvent &event) override;

    void copy_buffer_to_buffer(
        CommandBufferId, BufferTargetDescriptor src, BufferTargetDescriptor dst, usize size) const override;
    void copy_buffer_to_texture(
        CommandBufferId, BufferTargetDescriptor src, TextureTargetDescriptor dst, Extent3d extent) const override;
    void copy_texture_to_buffer(
        CommandBufferId, TextureTargetDescriptor src, BufferTargetDescriptor dst, Extent3d extent) const override;
    void copy_texture_to_texture(
        CommandBufferId, TextureTargetDescriptor src, TextureTargetDescriptor dst, Extent3d extent) const override;

    void push_constants(CommandBufferId, PipelineLayoutId, const void *data, usize size) override;

    // Compute Pass
    void begin_compute_pass(CommandBufferId, const ComputePassDescriptor &) const override;
    void end_compute_pass(CommandBufferId) const override;

    void bind_compute_pipeline(CommandBufferId, ComputePipelineId) const override;

    void dispatch(CommandBufferId, u32 x, u32 y, u32 z) const override;

    // Render Pass
    void begin_render_pass(CommandBufferId, const RenderPassDescriptor &) const override;
    void end_render_pass(CommandBufferId) const override;

    void bind_render_pipeline(CommandBufferId, RenderPipelineId) const override;
    void bind_index_buffer(CommandBufferId, BufferId) const override;

    void set_viewport(CommandBufferId, f32 x, f32 y, f32 width, f32 height, f32 min_depth, f32 max_depth) const override;
    void set_scissor(CommandBufferId, Offset2d, Extent2d) const override;
    void draw(CommandBufferId, u32 vertex_count, u32 instance_count, u32 first_vertex, u32 first_instance) const override;
    void draw_indexed(CommandBufferId,
        u32 index_count,
        u32 instance_count,
        u32 first_index,
        i32 vertex_offset,
        u32 first_instance) const override;

private:
    VulkanRenderDriver(const Instance &instance,
        const Device &device,
        const VmaAllocator allocator,
        const VkSurfaceKHR surface,
        Swapchain swapchain,
        const Descriptors &descriptors)
        : m_instance { instance }
        , m_device { device }
        , m_allocator { allocator }
        , m_surface { surface }
        , m_swapchain { std::move(swapchain) }
        , m_descriptors { descriptors }
    {
    }

    static std::vector<std::string> find_instance_layers();
    static std::unordered_set<std::string> find_instance_extensions();
    static Instance create_instance();

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT,
        VkDebugUtilsMessageTypeFlagsEXT,
        const VkDebugUtilsMessengerCallbackDataEXT *,
        void *);

    static std::optional<std::unordered_set<std::string>> check_device_extensions(VkPhysicalDevice);
    static std::optional<PhysicalDevice::FeatureSet> check_device_features(VkPhysicalDevice);
    static std::optional<u32> check_device_queues(VkInstance, VkPhysicalDevice);
    static bool check_device_limits(VkPhysicalDevice);
    static std::string_view map_device_type(VkPhysicalDeviceType);
    static u32 rate_device_type(VkPhysicalDeviceType);
    static PhysicalDevice choose_physical_device(const Instance &);

    static Device create_device(const Instance &, const PhysicalDevice &);
    static VmaAllocator create_allocator(const Instance &, const Device &);

    static VkSurfaceKHR create_surface(const Instance &, void *native_window);

    static Extent2d choose_extent(const Device &, VkSurfaceKHR, u32 width, u32 height);
    static VkSurfaceFormatKHR choose_surface_format(const Device &, VkSurfaceKHR);
    static VkPresentModeKHR choose_present_mode(const Device &, VkSurfaceKHR);
    static Swapchain create_swapchain(const Device &, VkSurfaceKHR, u32 width, u32 height, VkSwapchainKHR old);
    void recreate_swapchain();
    void destroy_swapchain();

    static VkDescriptorSetLayout create_descriptor_set_layout(const Device &, VkDescriptorType, u32 descriptor_count);
    static VkDescriptorSet create_descriptor_set(
        const Device &, VkDescriptorPool, u32 descriptor_count, VkDescriptorSetLayout);
    static VkDescriptorType map_descriptor_type(DescriptorType);
    static Descriptors create_descriptors(const Device &);

    static void transition_texture_layout(VkCommandBuffer, VulkanTexture *, VkImageLayout new_layout);

private:
    Instance m_instance {};
    Device m_device {};
    VmaAllocator m_allocator { VK_NULL_HANDLE };
    VkSurfaceKHR m_surface { VK_NULL_HANDLE };
    Swapchain m_swapchain {};
    Descriptors m_descriptors {};

    ShaderCompiler m_compiler { ShaderCompiler::CompilerTarget::Spirv };
};
