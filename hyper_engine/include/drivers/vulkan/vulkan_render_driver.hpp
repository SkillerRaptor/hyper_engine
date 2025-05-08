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
#include "systems/window/window_events.hpp"

class VulkanRenderDriver final : public RenderDriver
{
private:
    static constexpr const char *s_validation_layer = "VK_LAYER_KHRONOS_validation";

    static constexpr std::array<const char *, 1> s_device_extensions = {
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    static constexpr std::array<VkDescriptorType, 4> s_descriptor_types = {
        VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
        VK_DESCRIPTOR_TYPE_SAMPLER,
    };

    static constexpr size_t s_descriptor_count = s_descriptor_types.size();
    static constexpr uint32_t s_descriptor_limit = 1000 * 1000;

private:
    struct VulkanBuffer : Buffer
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
    };

    struct VulkanShader : Shader
    {
        VkShaderModule shader_module = VK_NULL_HANDLE;
    };

    struct VulkanSampler : Sampler
    {
        VkSampler sampler = VK_NULL_HANDLE;
    };

    struct VulkanTexture : Texture
    {
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        VkImageView image_view = VK_NULL_HANDLE;
        VkImageLayout image_layout = VK_IMAGE_LAYOUT_UNDEFINED;
    };

    struct VulkanPipelineLayout : PipelineLayout
    {
        VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    };

    struct VulkanComputePipeline : ComputePipeline
    {
        VkPipeline pipeline = VK_NULL_HANDLE;
    };

    struct VulkanRenderPipeline : RenderPipeline
    {
        VkPipeline pipeline = VK_NULL_HANDLE;
    };

    struct VulkanCommandBuffer : CommandBuffer
    {
        VkCommandPool command_pool = VK_NULL_HANDLE;
        VkCommandBuffer command_buffer = VK_NULL_HANDLE;
        VkFence render_fence = VK_NULL_HANDLE;
        VkSemaphore submit_semaphore = VK_NULL_HANDLE;
        uint64_t semaphore_counter = 0;
    };

public:
    void initialize(WindowSystem &window_system, WindowId window) override;
    void shutdown() override;

    void wait_idle() const override;

    std::vector<Texture *> query_swapchain_textures() override;

    // Buffer
    Buffer *create_buffer(const std::optional<std::string> &label, uint64_t byte_size, BitFlags<BufferUsage> usage, bool staging) const override;
    void destroy_buffer(const Buffer *buffer) const override;

    void *map_buffer(const Buffer *buffer) const override;
    void unmap_buffer(const Buffer *buffer) const override;

    // Shader
    Shader *
        create_shader(const std::optional<std::string> &label, ShaderType type, std::string_view entry, std::string_view path) const override;
    void destroy_shader(const Shader *shader) const override;

    // Sampler
    Sampler *create_sampler(
        const std::optional<std::string> &label,
        Filter mag_filter,
        Filter min_filter,
        Filter mipmap_filter,
        AddressMode address_mode_u,
        AddressMode address_mode_v,
        AddressMode address_mode_w,
        float mip_lod_bias,
        CompareOperation compare_operation,
        float min_lod,
        float max_lod,
        BorderColor border_color) const override;
    void destroy_sampler(const Sampler *sampler) const override;

    // Textures
    Texture *create_texture(
        const std::optional<std::string> &label,
        uint32_t width,
        uint32_t height,
        uint32_t depth,
        uint32_t array_size,
        uint32_t mip_levels,
        Format format,
        Dimension dimension,
        BitFlags<TextureUsage> usage) const override;
    void destroy_texture(const Texture *texture) const override;

    // Pipeline Layout
    PipelineLayout *create_pipeline_layout(const std::optional<std::string> &label, uint32_t push_constant_size) const override;
    void destroy_pipeline_layout(const PipelineLayout *pipeline_layout) const override;

    // Compute Pipeline
    ComputePipeline *
        create_compute_pipeline(const std::optional<std::string> &label, const PipelineLayout *layout, const Shader *shader) const override;
    void destroy_compute_pipeline(const ComputePipeline *compute_pipeline) const override;

    // Render Pipeline
    RenderPipeline *create_render_pipeline(
        const std::optional<std::string> &label,
        const PipelineLayout *layout,
        const Shader *vertex_shader,
        const Shader *fragment_shader,
        const std::vector<ColorAttachmentState> &color_attachment_states,
        const PrimitiveState &primitive_state,
        const DepthStencilState &depth_stencil_state) const override;
    void destroy_render_pipeline(const RenderPipeline *render_pipeline) const override;

    // Command Buffer
    CommandBuffer *create_command_buffer() const override;
    void destroy_command_buffer(const CommandBuffer *command_buffer) const override;

    void acquire_command_buffer(const CommandBuffer *command_buffer) const override;
    void submit_command_buffer(CommandBuffer *command_buffer) const override;

    void bind_buffer(const Buffer *buffer) const override;
    void bind_sampler(const Sampler *sampler) const override;
    void bind_texture(const Texture *texture) const override;

    void copy_buffer_to_buffer(
        const CommandBuffer *command_buffer,
        const Buffer *src,
        uint32_t src_offset,
        const Buffer *dst,
        uint32_t dst_offset,
        uint32_t size) const override;
    void copy_buffer_to_texture(
        const CommandBuffer *command_buffer,
        const Buffer *src,
        uint32_t src_offset,
        Texture *dst,
        Offset3d dst_offset,
        Extent3d dst_extent,
        uint32_t dst_mip_level,
        uint32_t dst_array_index) const override;
    void copy_texture_to_buffer(
        const CommandBuffer *command_buffer,
        Texture *src,
        Offset3d src_offset,
        Extent3d src_extent,
        uint32_t src_mip_level,
        uint32_t src_array_index,
        const Buffer *dst,
        uint32_t dst_offset) const override;
    void copy_texture_to_texture(
        const CommandBuffer *command_buffer,
        Texture *src,
        Offset3d src_offset,
        uint32_t src_mip_level,
        uint32_t src_array_index,
        Texture *dst,
        Offset3d dst_offset,
        uint32_t dst_mip_level,
        uint32_t dst_array_index,
        Extent3d extent) const override;

    void push_constants(const CommandBuffer *command_buffer, const PipelineLayout *pipeline_layout, const void *data, uint32_t size) override;

    std::pair<uint32_t, bool> acquire_swapchain_texture(const CommandBuffer *command_buffer) override;
    void present() override;

    void begin_gpu_marker(const CommandBuffer *command_buffer, Label label) const override;
    void end_gpu_marker(const CommandBuffer *command_buffer) const override;

    // Compute Pass
    void begin_compute_pass(const CommandBuffer *command_buffer) const override;
    void end_compute_pass(const CommandBuffer *command_buffer) const override;

    void bind_compute_pipeline(const CommandBuffer *command_buffer, const ComputePipeline *pipeline) const override;

    // Render Pass
    void begin_render_pass(
        const CommandBuffer *command_buffer,
        const std::vector<RenderPassColorAttachment> &color_attachments,
        const std::optional<RenderPassDepthStencilAttachment> &depth_stencil_attachment) const override;
    void end_render_pass(const CommandBuffer *command_buffer) const override;

    void bind_render_pipeline(const CommandBuffer *command_buffer, const RenderPipeline *pipeline) const override;
    void bind_index_buffer(const CommandBuffer *command_buffer, const Buffer *buffer) const override;

    void set_viewport(const CommandBuffer *command_buffer, float x, float y, float width, float height, float min_depth, float max_depth)
        const override;
    void set_scissor(const CommandBuffer *command_buffer, int32_t x, int32_t y, uint32_t width, uint32_t height) const override;
    void
        draw(const CommandBuffer *command_buffer, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
            const override;
    void draw_indexed(
        const CommandBuffer *command_buffer,
        uint32_t index_count,
        uint32_t instance_count,
        uint32_t first_index,
        int32_t vertex_offset,
        uint32_t first_instance) const override;

private:
    void create_instance();
    void create_debug_messenger();
    void choose_physical_device();
    uint32_t rate_physical_device(const VkPhysicalDevice &physical_device) const;
    std::optional<uint32_t> find_queue_family(const VkPhysicalDevice &physical_device) const;
    void create_device();
    void create_allocator();

    void create_surface(const WindowSystem &window_system, WindowId id);
    void create_swapchain(uint32_t width, uint32_t height);

    void recreate_swapchain();
    void destroy_swapchain();

    void on_resize(const WindowResizeEvent &event);

    void find_descriptor_counts();
    void create_descriptor_pool();
    void create_descriptor_set_layouts();
    void create_descriptor_sets();

    VkImageView create_internal_image_view(const VkImage image, const Dimension dimension, const Format format) const;

    static bool is_validation_layer_supported();
    static bool check_extension_support(const VkPhysicalDevice &physical_device);
    static bool check_feature_support(const VkPhysicalDevice &physical_device);

    static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
        VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
        VkDebugUtilsMessageTypeFlagsEXT message_type,
        const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
        void *);

    static void transition_texture_layout(const CommandBuffer *command_buffer, Texture *texture, VkImageLayout new_layout);

    static VkExtent2D choose_extent(uint32_t width, uint32_t height, const VkSurfaceCapabilitiesKHR &capabilities);
    static VkSurfaceFormatKHR choose_format(const std::vector<VkSurfaceFormatKHR> &formats);
    static VkPresentModeKHR choose_present_mode(const std::vector<VkPresentModeKHR> &present_modes);

    static VkBufferUsageFlags get_buffer_usage_flags(BitFlags<BufferUsage> buffer_usage_flags);

    static VkFilter get_filter(Filter filter);
    static VkSamplerMipmapMode get_sampler_mipmap_mode(Filter filter);
    static VkSamplerAddressMode get_sampler_address_mode(AddressMode filter);
    static VkBorderColor get_border_color(BorderColor border_color);

    static Format format_to_texture_format(VkFormat format);
    static VkFormat get_format(Format format);
    static VkImageType get_image_type(Dimension dimension);
    static VkImageUsageFlags get_image_usage_flags(BitFlags<TextureUsage> texture_usage_flags, Format format);
    static VkImageAspectFlags get_image_aspect_flags(Format format);
    static VkImageViewType get_image_view_type(Dimension dimension);

    static VkPrimitiveTopology get_primitive_topology(PrimitiveTopology primitive_topology);
    static VkPolygonMode get_polygon_mode(PolygonMode polygon_mode);
    static VkCullModeFlags get_cull_mode_flags(Face face);
    static VkFrontFace get_front_face(FrontFace front_face);
    static VkCompareOp get_compare_operation(CompareOperation compare_operation);
    static VkBlendFactor get_blend_factor(BlendFactor blend_factor);
    static VkBlendOp get_blend_operation(BlendOperation blend_operation);
    static VkColorComponentFlags get_color_component_flags(BitFlags<ColorWrites> color_writes);

    static VkAttachmentLoadOp get_attachment_load_operation(LoadOperation load_operation);
    static VkAttachmentStoreOp get_attachment_store_operation(StoreOperation store_operation);

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
    uint32_t m_swapchain_width = 0;
    uint32_t m_swapchain_height = 0;
    uint32_t m_min_image_count = 0;
    uint32_t m_image_count = 0;
    VkFormat m_swapchain_format = VK_FORMAT_UNDEFINED;
    std::vector<Texture *> m_swapchain_textures;

    bool m_swapchain_out_of_date = false;
    uint32_t m_swapchain_texture_index = 0;

    std::array<uint32_t, s_descriptor_count> m_descriptor_counts = {};

    VkDescriptorPool m_descriptor_pool = VK_NULL_HANDLE;
    std::array<VkDescriptorSetLayout, s_descriptor_count> m_descriptor_set_layouts = {};
    std::array<VkDescriptorSet, s_descriptor_count> m_descriptor_sets = {};
};
