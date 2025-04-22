/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>

// clang-format off
#include <volk.h>
#include <vk_mem_alloc.h>
// clang-format on

#include "systems/render/render_driver.hpp"
#include "systems/render/shader_compiler.hpp"
#include "systems/render_system.hpp"

class VulkanRenderDriver final : public RenderDriver
{
private:
    static constexpr const char *s_validation_layer = "VK_LAYER_KHRONOS_validation";

    static constexpr std::array<const char *, 1> s_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

private:
    struct VulkanBuffer : RS::Buffer
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
    };

    struct VulkanShader : RS::Shader
    {
        VkShaderModule shader_module = VK_NULL_HANDLE;
        VkPipelineShaderStageCreateInfo pipeline_stage_create_info = {};
    };

    struct VulkanSampler : RS::Sampler
    {
        VkSampler sampler = VK_NULL_HANDLE;
    };

    struct VulkanTexture : RS::Texture
    {
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        VkImageView image_view = VK_NULL_HANDLE;
        VkImageLayout image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    };

    struct VulkanPipelineLayout : RS::PipelineLayout
    {
        VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    };

    struct VulkanComputePipeline : RS::ComputePipeline
    {
        VkPipeline pipeline = VK_NULL_HANDLE;
    };

    struct VulkanRenderPipeline : RS::RenderPipeline
    {
        VkPipeline pipeline = VK_NULL_HANDLE;
    };

    struct VulkanCommandBuffer : RS::CommandBuffer
    {
        VkCommandPool command_pool = VK_NULL_HANDLE;
        VkCommandBuffer command_buffer = VK_NULL_HANDLE;
        VkFence render_fence = VK_NULL_HANDLE;
        VkSemaphore submit_semaphore = VK_NULL_HANDLE;
        uint64_t semaphore_counter = 0;
    };

public:
    ~VulkanRenderDriver() override;

    void initialize(const WindowSystem &window_system, WindowSystem::WindowId window) override;
    std::vector<RS::Texture *> query_swapchain_textures() override;

    // Buffer
    RS::Buffer *create_buffer(const std::optional<std::string> &label, uint64_t byte_size, BitFlags<RS::BufferUsage> usage) override;
    RS::Buffer *create_staging_buffer(const std::optional<std::string> &label, uint64_t byte_size, BitFlags<RS::BufferUsage> usage) override;
    void destroy_buffer(RS::Buffer *buffer) override;

    // Shader
    RS::Shader *
        create_shader(const std::optional<std::string> &label, RS::ShaderType type, const std::string &entry, const std::string &path) override;
    void destroy_shader(RS::Shader *shader) override;

    // Sampler
    RS::Sampler *create_sampler(
        const std::optional<std::string> &label,
        RS::Filter mag_filter,
        RS::Filter min_filter,
        RS::Filter mipmap_filter,
        RS::AddressMode address_mode_u,
        RS::AddressMode address_mode_v,
        RS::AddressMode address_mode_w,
        float mip_lod_bias,
        RS::CompareOperation compare_operation,
        float min_lod,
        float max_lod,
        RS::BorderColor border_color) override;
    void destroy_sampler(RS::Sampler *sampler) override;

    // Textures
    RS::Texture *create_texture(
        const std::optional<std::string> &label,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint32_t array_size,
        uint32_t mip_levels,
        RS::Format format,
        RS::Dimension dimension,
        BitFlags<RS::TextureUsage> usage) override;
    void destroy_texture(RS::Texture *texture) override;

    // Pipeline Layout
    RS::PipelineLayout *create_pipeline_layout(const std::optional<std::string> &label, uint32_t push_constant_size) override;
    void destroy_pipeline_layout(RS::PipelineLayout *pipeline_layout) override;

    // Compute Pipeline
    RS::ComputePipeline *
        create_compute_pipeline(const std::optional<std::string> &label, RS::PipelineLayout *layout, RS::Shader *shader) override;
    void destroy_compute_pipeline(RS::ComputePipeline *compute_pipeline) override;

    // Render Pipeline
    RS::RenderPipeline *create_render_pipeline(
        const std::optional<std::string> &label,
        RS::PipelineLayout *layout,
        RS::Shader *vertex_shader,
        RS::Shader *fragment_shader,
        const std::vector<RS::ColorAttachmentState> &color_attachment_states,
        const RS::PrimitiveState &primitive_state,
        const RS::DepthStencilState &depth_stencil_state) override;
    void destroy_render_pipeline(RS::RenderPipeline *render_pipeline) override;

    // Command Buffer
    RS::CommandBuffer *create_command_buffer() override;
    void destroy_command_buffer(RS::CommandBuffer *command_buffer) override;

    void acquire_command_buffer(RS::CommandBuffer *command_buffer) override;
    void submit_command_buffer(RS::CommandBuffer *command_buffer) override;

    uint32_t acquire_swapchain_texture(RS::CommandBuffer *command_buffer) override;
    void present() override;

    void begin_gpu_marker(RS::CommandBuffer *command_buffer, std::string_view name, RS::LabelColor label_color) const override;
    void end_gpu_marker(RS::CommandBuffer *command_buffer) const override;

    // Compute Pass
    void begin_compute_pass(RS::CommandBuffer *command_buffer) override;
    void end_compute_pass(RS::CommandBuffer *command_buffer) override;

    void bind_compute_pipeline(RS::CommandBuffer *command_buffer, RS::ComputePipeline *pipeline) override;

    // Render Pass
    void begin_render_pass(RS::CommandBuffer *command_buffer, const RS::RenderPassDescriptor &descriptor, RS::Texture *texture) override;
    void end_render_pass(RS::CommandBuffer *command_buffer) override;

    void bind_render_pipeline(RS::CommandBuffer *command_buffer, RS::RenderPipeline *pipeline) override;

    void set_viewport(RS::CommandBuffer *command_buffer, const RS::Viewport &viewport) override;
    void set_scissor(RS::CommandBuffer *command_buffer, const RS::Scissor &scissor) override;
    void draw(RS::CommandBuffer *command_buffer, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
        override;

private:
    void create_instance();
    void create_debug_messenger();
    void choose_physical_device();
    uint32_t rate_physical_device(const VkPhysicalDevice &physical_device) const;
    std::optional<uint32_t> find_queue_family(const VkPhysicalDevice &physical_device) const;
    void create_device();
    void create_allocator();

    void create_surface(const WS &window_system, WS::WindowId id);
    void create_swapchain(const WS &window_system, const WS::WindowId id);

    RS::Buffer *
        create_internal_buffer(const std::optional<std::string> &label, uint64_t byte_size, BitFlags<RS::BufferUsage> usage, bool staging) const;

    void transition_texture_layout(RS::CommandBuffer *command_buffer, RS::Texture *texture, VkImageLayout new_layout);

    static bool is_validation_layer_supported();
    static bool check_extension_support(const VkPhysicalDevice &physical_device);
    static bool check_feature_support(const VkPhysicalDevice &physical_device);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void *);

    static VkExtent2D choose_extent(uint32_t width, uint32_t height, const VkSurfaceCapabilitiesKHR &capabilities);
    static VkSurfaceFormatKHR choose_format(const std::vector<VkSurfaceFormatKHR> &formats);
    static VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR> &present_modes);

    static VkBufferUsageFlags get_buffer_usage_flags(BitFlags<RS::BufferUsage> buffer_usage_flags);

    static VkFilter get_filter(RS::Filter filter);
    static VkSamplerMipmapMode get_sampler_mipmap_mode(RS::Filter filter);
    static VkSamplerAddressMode get_sampler_address_mode(RS::AddressMode filter);
    static VkBorderColor get_border_color(RS::BorderColor border_color);

    static RS::Format format_to_texture_format(VkFormat format);
    static VkFormat get_format(RS::Format format);
    static VkImageType get_image_type(RS::Dimension dimension);
    static VkImageUsageFlags get_image_usage_flags(BitFlags<RS::TextureUsage> texture_usage_flags, RS::Format format);
    static VkImageAspectFlags get_image_aspect_flags(RS::Format format);
    static VkImageViewType get_image_view_type(RS::Dimension dimension);

    static VkPrimitiveTopology get_primitive_topology(RS::PrimitiveTopology primitive_topology);
    static VkPolygonMode get_polygon_mode(RS::PolygonMode polygon_mode);
    static VkCullModeFlags get_cull_mode_flags(RS::Face face);
    static VkFrontFace get_front_face(RS::FrontFace front_face);
    static VkCompareOp get_compare_operation(RS::CompareOperation compare_operation);
    static VkBlendFactor get_blend_factor(RS::BlendFactor blend_factor);
    static VkBlendOp get_blend_operation(RS::BlendOperation blend_operation);
    static VkColorComponentFlags get_color_component_flags(BitFlags<RS::ColorWrites> color_writes);

private:
    ShaderCompiler m_compiler = ShaderCompiler(ShaderCompiler::CompilerTarget::Spirv);

    bool m_validation_layer_enabled = false;
    VkInstance m_instance = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
    VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    uint32_t m_queue_family = 0;
    VkQueue m_queue = VK_NULL_HANDLE;
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    uint32_t m_min_image_count = 0;
    uint32_t m_image_count = 0;
    VkFormat m_swapchain_format = VK_FORMAT_UNDEFINED;
    std::vector<RS::Texture *> m_swapchain_textures;

    bool m_swapchain_out_of_date = false;
    uint32_t m_swapchain_texture_index = 0;
};
