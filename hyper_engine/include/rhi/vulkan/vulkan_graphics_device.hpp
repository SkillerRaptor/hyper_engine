/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <array>
#include <optional>
#include <vector>

#include "hyper_rhi/label_color.hpp"
#include "hyper_rhi/graphics_device.hpp"
#include "hyper_rhi/resource_handle.hpp"
#include "hyper_rhi/vulkan/vulkan_common.hpp"

#include <vk_mem_alloc.h>

namespace hyper_engine
{
    class Logger;
    class VulkanDescriptorManager;

    enum class ObjectType
    {
        Buffer,
        CommandPool,
        Fence,
        Image,
        ImageView,
        Pipeline,
        PipelineLayout,
        Queue,
        Sampler,
        ShaderModule,
        Semaphore,
    };

    enum class MarkerType
    {
        ComputePass,
        RenderPass
    };

    struct BufferEntry
    {
        VkBuffer buffer = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        ResourceHandle handle = {};
    };

    struct TextureEntry
    {
        VkImage image = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
    };

    struct TextureViewEntry
    {
        VkImageView view = VK_NULL_HANDLE;
        ResourceHandle handle = {};
    };

    struct ResourceQueue
    {
        std::vector<BufferEntry> buffers;
        std::vector<VkPipeline> compute_pipelines;
        std::vector<VkPipeline> graphics_pipelines;
        std::vector<VkPipelineLayout> pipeline_layouts;
        std::vector<VkSampler> samplers;
        std::vector<VkShaderModule> shader_modules;
        std::vector<TextureEntry> textures;
        std::vector<TextureViewEntry> texture_views;
    };

    struct FrameData
    {
        VkCommandPool command_pool;
        VkCommandBuffer command_buffer;

        VkFence render_fence;
        VkSemaphore submit_semaphore;
        uint64_t semaphore_counter;
    };

    class VulkanGraphicsDevice final : public GraphicsDevice
    {
    public:
        VulkanGraphicsDevice(const Logger &logger, const GraphicsDeviceDescriptor &descriptor);
        ~VulkanGraphicsDevice() override;

        std::shared_ptr<Surface> create_surface(SDL_Window *window) override;
        std::shared_ptr<CommandList> create_command_list() override;

        std::shared_ptr<Buffer> create_buffer_platform(const BufferDescriptor &descriptor, ResourceHandle handle) override;
        std::shared_ptr<Buffer> create_buffer_internal(const BufferDescriptor &descriptor, ResourceHandle handle, bool staging);

        std::shared_ptr<ComputePipeline> create_compute_pipeline_platform(const ComputePipelineDescriptor &descriptor) override;
        std::shared_ptr<RenderPipeline> create_render_pipeline_platform(const RenderPipelineDescriptor &descriptor) override;
        std::shared_ptr<PipelineLayout> create_pipeline_layout_platform(const PipelineLayoutDescriptor &descriptor) override;
        std::shared_ptr<ShaderModule> create_shader_module_platform(const ShaderModuleDescriptor &descriptor) override;

        std::shared_ptr<Sampler> create_sampler_platform(const SamplerDescriptor &descriptor, ResourceHandle handle) override;
        std::shared_ptr<Texture> create_texture_platform(const TextureDescriptor &descriptor) override;
        std::shared_ptr<Texture> create_texture_internal(const TextureDescriptor &descriptor, VkImage image);
        std::shared_ptr<TextureView> create_texture_view_platform(const TextureViewDescriptor &descriptor, ResourceHandle handle) override;

        void begin_marker(VkCommandBuffer command_buffer, MarkerType type, std::string_view name, LabelColor color) const;
        void end_marker(VkCommandBuffer command_buffer) const;

        void set_object_name(const void *handle, ObjectType type, std::string_view name) const;
        void destroy_resources();

        void begin_frame(std::shared_ptr<Surface> &surface, uint32_t frame_index) override;
        void end_frame() const override;
        void execute(const std::shared_ptr<CommandList> &command_list) override;
        void present(const std::shared_ptr<Surface> &surface) const override;

        void wait_for_idle() const override;

        GraphicsApi graphics_api() const override;
        bool debug_validation() const override;
        bool debug_label() const override;
        bool debug_marker() const override;

        DescriptorManager &descriptor_manager() override;

        VkInstance instance() const;
        VkPhysicalDevice physical_device() const;
        VkDevice device() const;
        uint32_t queue_family() const;
        VkQueue queue() const;
        VmaAllocator allocator() const;
        ResourceQueue &resource_queue();
        const FrameData &current_frame() const;
        uint32_t current_frame_index() const;

    private:
        void create_instance();
        void create_debug_messenger();
        void choose_physical_device();
        uint32_t rate_physical_device(const VkPhysicalDevice &physical_device) const;
        std::optional<uint32_t> find_queue_family(const VkPhysicalDevice &physical_device) const;
        void create_device();
        void create_allocator();
        void create_frames();

        static bool check_validation_layer_support();
        static bool check_extension_support(const VkPhysicalDevice &physical_device);
        static bool check_feature_support(const VkPhysicalDevice &physical_device);

        static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(
            VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
            VkDebugUtilsMessageTypeFlagsEXT message_type,
            const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
            void *);

    private:
        const Logger &m_logger;

        GraphicsApi m_graphics_api = GraphicsApi::Vulkan;
        bool m_debug_validation = false;
        bool m_debug_label = false;
        bool m_debug_marker = false;

        VkInstance m_instance = VK_NULL_HANDLE;
        VkDebugUtilsMessengerEXT m_debug_messenger = VK_NULL_HANDLE;
        VkPhysicalDevice m_physical_device = VK_NULL_HANDLE;
        VkDevice m_device = VK_NULL_HANDLE;
        uint32_t m_queue_family = 0;
        VkQueue m_queue = VK_NULL_HANDLE;
        VmaAllocator m_allocator = VK_NULL_HANDLE;

        // NOTE: Using raw pointer to guarantee order of destruction
        VulkanDescriptorManager *m_descriptor_manager = nullptr;

        uint32_t m_current_frame_index = 0;
        std::array<FrameData, GraphicsDevice::s_frame_count> m_frames;

        ResourceQueue m_resource_queue;
    };
} // namespace hyper_engine
