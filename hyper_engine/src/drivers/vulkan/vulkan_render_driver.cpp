/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "drivers/vulkan/vulkan_render_driver.hpp"

#include <algorithm>
#include <array>
#include <filesystem>
#include <map>
#include <ranges>
#include <set>
#include <vector>

#define VMA_IMPLEMENTATION
#include <SDL3/SDL_vulkan.h>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>

#include "core/filesystem.hpp"
#include "core/logger.hpp"
#include "core/prerequisites.hpp"

#define HE_VK_TYPE_TO_STRING(type, value) string_##type(value)

#define HE_VK_CHECK(result, fn)                                             \
    do                                                                      \
    {                                                                       \
        if ((result) != VK_SUCCESS)                                         \
        {                                                                   \
            const char *vk_result = HE_VK_TYPE_TO_STRING(VkResult, result); \
            HE_PANIC(HE_STRINGIFY(fn) " failed", vk_result);                \
        }                                                                   \
    } while (0)

VulkanRenderDriver::~VulkanRenderDriver()
{
    const VkResult result = vkDeviceWaitIdle(m_device);
    HE_VK_CHECK(result, vkDeviceWaitIdle);

    vmaDestroyAllocator(m_allocator);

    vkDestroyDevice(m_device, nullptr);

    if (m_validation_layer_enabled)
    {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
    }

    vkDestroyInstance(m_instance, nullptr);
}

void VulkanRenderDriver::initialize(const WindowSystem &window_system, const WindowSystem::WindowId window)
{
    const VkResult result = volkInitialize();
    HE_VK_CHECK(result, volkInitialize);

#ifdef HE_DEBUG_BUILD
    if (is_validation_layer_supported())
    {
        m_validation_layer_enabled = true;
    }
    else
    {
        HE_WARN("Validations layers request, but not available");
        m_validation_layer_enabled = false;
    }
#endif

    create_instance();

    if (m_validation_layer_enabled)
    {
        create_debug_messenger();
    }

    choose_physical_device();
    create_device();
    create_allocator();

    create_surface(window_system, window);
    create_swapchain(window_system, window);

    // FIXME: Add descriptors
    // FIXME: Add per frame structures

    HE_INFO("Successfully initialized VulkanRenderDriver");
}

std::vector<RS::Texture *> VulkanRenderDriver::query_swapchain_textures()
{
    return m_swapchain_textures;
}

RS::Buffer *
    VulkanRenderDriver::create_buffer(const std::optional<std::string> &label, const uint64_t byte_size, const BitFlags<RS::BufferUsage> usage)
{
    return create_internal_buffer(label, byte_size, usage, false);
}

RS::Buffer *VulkanRenderDriver::create_staging_buffer(
    const std::optional<std::string> &label,
    const uint64_t byte_size,
    const BitFlags<RS::BufferUsage> usage)
{
    return create_internal_buffer(label, byte_size, usage, true);
}

void VulkanRenderDriver::destroy_buffer(RS::Buffer *buffer)
{
    const VulkanBuffer *vulkan_buffer = reinterpret_cast<VulkanBuffer *>(buffer);
    vkDestroyBuffer(m_device, vulkan_buffer->buffer, nullptr);
    delete vulkan_buffer;
}

RS::Shader *VulkanRenderDriver::create_shader(
    const std::optional<std::string> &label,
    const RS::ShaderType type,
    const std::string &entry,
    const std::string &path)
{
    const std::vector<uint8_t> data = filesystem::read_file(path);
    HE_ASSERT(!data.empty());

    const ShaderCompilationDescriptor shader_compilation_descriptor = {
        .entry_name = entry,
        .type = type,
        .data = data,
    };

    const std::vector<uint8_t> code = m_compiler.compile(shader_compilation_descriptor);
    HE_ASSERT(!code.empty());

    if (!code.empty())
    {
        const std::string file_name = std::filesystem::path(path).filename().string();
        const std::string_view shader_stage = [type]()
        {
            switch (type)
            {
            case RS::ShaderType::Compute:
                return "compute";
            case RS::ShaderType::Fragment:
                return "fragment";
            case RS::ShaderType::Vertex:
                return "vertex";
            default:
                HE_UNREACHABLE();
            }
        }();

        HE_INFO("Compiled '{}' for {} stage successfully", file_name, shader_stage);
    }

    const VkShaderModuleCreateInfo shader_module_create_info = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t *>(code.data()),
    };

    VkShaderModule vk_shader_module = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &vk_shader_module), vkCreateShaderModule);

    HE_ASSERT(vk_shader_module != VK_NULL_HANDLE);

    const VkShaderStageFlagBits shader_stage_flag_bits = [type]()
    {
        switch (type)
        {
        case RS::ShaderType::Compute:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        case RS::ShaderType::Fragment:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case RS::ShaderType::Vertex:
            return VK_SHADER_STAGE_VERTEX_BIT;
        default:
            HE_UNREACHABLE();
        }
    }();
    const VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = shader_stage_flag_bits,
        .module = vk_shader_module,
        .pName = entry.c_str(),
        .pSpecializationInfo = nullptr,
    };

    return new VulkanShader({
        .shader_module = vk_shader_module,
        .pipeline_stage_create_info = pipeline_shader_stage_create_info,
    });
}

void VulkanRenderDriver::destroy_shader(RS::Shader *shader)
{
    const VulkanShader *vulkan_shader = reinterpret_cast<VulkanShader *>(shader);
    vkDestroyShaderModule(m_device, vulkan_shader->shader_module, nullptr);
    delete vulkan_shader;
}

RS::Sampler *VulkanRenderDriver::create_sampler(
    const std::optional<std::string> &label,
    const RS::Filter mag_filter,
    const RS::Filter min_filter,
    const RS::Filter mipmap_filter,
    const RS::AddressMode address_mode_u,
    const RS::AddressMode address_mode_v,
    const RS::AddressMode address_mode_w,
    const float mip_lod_bias,
    const RS::CompareOperation compare_operation,
    const float min_lod,
    const float max_lod,
    const RS::BorderColor border_color)
{
    const VkSamplerCreateInfo sampler_create_info = {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = get_filter(mag_filter),
        .minFilter = get_filter(min_filter),
        .mipmapMode = get_sampler_mipmap_mode(mipmap_filter),
        .addressModeU = get_sampler_address_mode(address_mode_u),
        .addressModeV = get_sampler_address_mode(address_mode_v),
        .addressModeW = get_sampler_address_mode(address_mode_w),
        .mipLodBias = mip_lod_bias,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 0.0,
        .compareEnable = VK_TRUE,
        .compareOp = get_compare_operation(compare_operation),
        .minLod = min_lod,
        .maxLod = max_lod,
        .borderColor = get_border_color(border_color),
        .unnormalizedCoordinates = VK_FALSE,
    };

    VkSampler vk_sampler = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateSampler(m_device, &sampler_create_info, nullptr, &vk_sampler), vkCreateSampler);

    HE_ASSERT(vk_sampler != VK_NULL_HANDLE);

    return new VulkanSampler({
        .sampler = vk_sampler,
    });
}

void VulkanRenderDriver::destroy_sampler(RS::Sampler *sampler)
{
    const VulkanSampler *vulkan_sampler = reinterpret_cast<VulkanSampler *>(sampler);
    vkDestroySampler(m_device, vulkan_sampler->sampler, nullptr);
    delete vulkan_sampler;
}

RS::Texture *VulkanRenderDriver::create_texture(
    const std::optional<std::string> &label,
    const uint32_t width,
    const uint32_t height,
    const uint32_t depth,
    const uint32_t array_size,
    const uint32_t mip_levels,
    const RS::Format format,
    const RS::Dimension dimension,
    const BitFlags<RS::TextureUsage> usage)
{
    (void) label;
    (void) width;
    (void) height;
    (void) depth;
    (void) array_size;
    (void) mip_levels;
    (void) format;
    (void) dimension;
    (void) usage;

    // FIXME: Implement this
    HE_PANIC("TODO: Implement `VulkanRenderDriver::create_texture`");

    return nullptr;
}

void VulkanRenderDriver::destroy_texture(RS::Texture *texture)
{
    (void) texture;

    // FIXME: Implement this
    HE_PANIC("TODO: Implement `VulkanRenderDriver::destroy_texture`");
}

RS::PipelineLayout *VulkanRenderDriver::create_pipeline_layout(const std::optional<std::string> &label, const uint32_t push_constant_size)
{
    const VkPushConstantRange push_constant_range = {
        .stageFlags = VK_SHADER_STAGE_ALL,
        .offset = 0,
        .size = push_constant_size,
    };

    // FIXME: Add descriptors
    const VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = static_cast<uint32_t>(push_constant_size == 0 ? 0 : 1),
        .pPushConstantRanges = push_constant_size == 0 ? nullptr : &push_constant_range,
    };

    VkPipelineLayout vk_pipeline_layout = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &vk_pipeline_layout), vkCreatePipelineLayout);

    HE_ASSERT(vk_pipeline_layout != VK_NULL_HANDLE);

    return new VulkanPipelineLayout({
        .pipeline_layout = vk_pipeline_layout,
    });
}

void VulkanRenderDriver::destroy_pipeline_layout(RS::PipelineLayout *pipeline_layout)
{
    const VulkanPipelineLayout *vulkan_pipeline_layout = reinterpret_cast<VulkanPipelineLayout *>(pipeline_layout);
    vkDestroyPipelineLayout(m_device, vulkan_pipeline_layout->pipeline_layout, nullptr);
    delete vulkan_pipeline_layout;
}

RS::ComputePipeline *
    VulkanRenderDriver::create_compute_pipeline(const std::optional<std::string> &label, RS::PipelineLayout *layout, RS::Shader *shader)
{
    const VulkanPipelineLayout *vulkan_layout = reinterpret_cast<VulkanPipelineLayout *>(layout);
    const VulkanShader *vulkan_shader = reinterpret_cast<VulkanShader *>(shader);

    const VkComputePipelineCreateInfo compute_pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = vulkan_shader->pipeline_stage_create_info,
        .layout = vulkan_layout->pipeline_layout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipeline vk_pipeline = VK_NULL_HANDLE;
    HE_VK_CHECK(
        vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &vk_pipeline), vkCreateComputePipelines);

    HE_ASSERT(vk_pipeline != VK_NULL_HANDLE);

    return new VulkanComputePipeline({
        .pipeline = vk_pipeline,
    });
}

void VulkanRenderDriver::destroy_compute_pipeline(RS::ComputePipeline *compute_pipeline)
{
    const VulkanComputePipeline *vulkan_compute_pipeline = reinterpret_cast<VulkanComputePipeline *>(compute_pipeline);
    vkDestroyPipeline(m_device, vulkan_compute_pipeline->pipeline, nullptr);
    delete vulkan_compute_pipeline;
}

RS::RenderPipeline *VulkanRenderDriver::create_render_pipeline(
    const std::optional<std::string> &label,
    RS::PipelineLayout *layout,
    RS::Shader *vertex_shader,
    RS::Shader *fragment_shader,
    const std::vector<RS::ColorAttachmentState> &color_attachment_states,
    const RS::PrimitiveState &primitive_state,
    const RS::DepthStencilState &depth_stencil_state)
{
    const VulkanShader *vulkan_vertex_shader = reinterpret_cast<VulkanShader *>(vertex_shader);
    const VulkanShader *vulkan_fragment_shader = reinterpret_cast<VulkanShader *>(fragment_shader);

    const std::array<VkPipelineShaderStageCreateInfo, 2> shader_stage_create_infos = {
        vulkan_vertex_shader->pipeline_stage_create_info,
        vulkan_fragment_shader->pipeline_stage_create_info,
    };

    constexpr VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };

    const VkPrimitiveTopology primitive_topology = get_primitive_topology(primitive_state.topology);
    const VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = primitive_topology,
        .primitiveRestartEnable = false,
    };

    constexpr VkPipelineTessellationStateCreateInfo tessellation_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .patchControlPoints = 0,
    };

    constexpr VkPipelineViewportStateCreateInfo viewport_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr,
    };

    const VkPolygonMode polygon_mode = get_polygon_mode(primitive_state.polygon_mode);
    const VkCullModeFlags cull_mode = get_cull_mode_flags(primitive_state.cull_mode);
    const VkFrontFace front_face = get_front_face(primitive_state.front_face);
    const VkPipelineRasterizationStateCreateInfo rasterization_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = false,
        .rasterizerDiscardEnable = false,
        .polygonMode = polygon_mode,
        .cullMode = cull_mode,
        .frontFace = front_face,
        .depthBiasEnable = depth_stencil_state.depth_bias_state.depth_bias_enable,
        .depthBiasConstantFactor = depth_stencil_state.depth_bias_state.constant,
        .depthBiasClamp = depth_stencil_state.depth_bias_state.clamp,
        .depthBiasSlopeFactor = depth_stencil_state.depth_bias_state.slope,
        .lineWidth = 1.0,
    };

    // FIXME: Add multisampling
    constexpr VkPipelineMultisampleStateCreateInfo multisample_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = false,
        .minSampleShading = 1.0,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = false,
        .alphaToOneEnable = false,
    };

    // FIXME: Add stencil
    // FIXME: Add depth bounds
    const VkCompareOp depth_compare_operation = get_compare_operation(depth_stencil_state.depth_compare_operation);
    const VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = depth_stencil_state.depth_test_enable,
        .depthWriteEnable = depth_stencil_state.depth_write_enable,
        .depthCompareOp = depth_compare_operation,
        .depthBoundsTestEnable = false,
        .stencilTestEnable = false,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0,
        .maxDepthBounds = 1.0,
    };

    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states = {};
    for (const RS::ColorAttachmentState &color_attachment_state : color_attachment_states)
    {
        const RS::BlendState &blend_state = color_attachment_state.blend_state;

        const VkBlendFactor src_color_blend_factor = get_blend_factor(blend_state.src_blend_factor);
        const VkBlendFactor dst_color_blend_factor = get_blend_factor(blend_state.dst_blend_factor);
        const VkBlendOp color_blend_operation = get_blend_operation(blend_state.operation);
        const VkBlendFactor alpha_src_blend_factor = get_blend_factor(blend_state.alpha_src_blend_factor);
        const VkBlendFactor alpha_dst_blend_factor = get_blend_factor(blend_state.alpha_dst_blend_factor);
        const VkBlendOp alpha_blend_operation = get_blend_operation(blend_state.alpha_operation);
        const VkColorComponentFlags color_write_mask = get_color_component_flags(blend_state.color_writes);
        const VkPipelineColorBlendAttachmentState color_blend_attachment_state = {
            .blendEnable = blend_state.blend_enable,
            .srcColorBlendFactor = src_color_blend_factor,
            .dstColorBlendFactor = dst_color_blend_factor,
            .colorBlendOp = color_blend_operation,
            .srcAlphaBlendFactor = alpha_src_blend_factor,
            .dstAlphaBlendFactor = alpha_dst_blend_factor,
            .alphaBlendOp = alpha_blend_operation,
            .colorWriteMask = color_write_mask,
        };

        color_blend_attachment_states.push_back(color_blend_attachment_state);
    }

    const VkPipelineColorBlendStateCreateInfo color_blend_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = false,
        .logicOp = VK_LOGIC_OP_NO_OP,
        .attachmentCount = static_cast<uint32_t>(color_blend_attachment_states.size()),
        .pAttachments = color_blend_attachment_states.data(),
        .blendConstants =
            {
                0.0,
                0.0,
                0.0,
                0.0,
            },
    };

    constexpr std::array<VkDynamicState, 2> dynamic_states = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    const VkPipelineDynamicStateCreateInfo dynamic_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<uint32_t>(dynamic_states.size()),
        .pDynamicStates = dynamic_states.data(),
    };

    std::vector<VkFormat> color_attachment_formats = {};
    for (const RS::ColorAttachmentState &color_attachment_state : color_attachment_states)
    {
        color_attachment_formats.push_back(get_format(color_attachment_state.format));
    }

    const VkFormat depth_attachment_format = get_format(depth_stencil_state.depth_format);
    const VkPipelineRenderingCreateInfo rendering_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = static_cast<uint32_t>(color_attachment_formats.size()),
        .pColorAttachmentFormats = color_attachment_formats.data(),
        .depthAttachmentFormat = depth_attachment_format,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    const VulkanPipelineLayout *vulkan_pipeline_layout = reinterpret_cast<VulkanPipelineLayout *>(layout);

    const VkGraphicsPipelineCreateInfo graphics_pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &rendering_create_info,
        .flags = 0,
        .stageCount = static_cast<uint32_t>(shader_stage_create_infos.size()),
        .pStages = shader_stage_create_infos.data(),
        .pVertexInputState = &vertex_input_state_create_info,
        .pInputAssemblyState = &input_assembly_state_create_info,
        .pTessellationState = &tessellation_state_create_info,
        .pViewportState = &viewport_state_create_info,
        .pRasterizationState = &rasterization_state_create_info,
        .pMultisampleState = &multisample_state_create_info,
        .pDepthStencilState = &depth_stencil_state_create_info,
        .pColorBlendState = &color_blend_state_create_info,
        .pDynamicState = &dynamic_state_create_info,
        .layout = vulkan_pipeline_layout->pipeline_layout,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipeline vk_pipeline = VK_NULL_HANDLE;
    HE_VK_CHECK(
        vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &vk_pipeline),
        vkCreateGraphicsPipelines);

    HE_ASSERT(vk_pipeline != VK_NULL_HANDLE);

    return new VulkanRenderPipeline({
        .pipeline = vk_pipeline,
    });
}

void VulkanRenderDriver::destroy_render_pipeline(RS::RenderPipeline *render_pipeline)
{
    const VulkanRenderPipeline *vulkan_render_pipeline = reinterpret_cast<VulkanRenderPipeline *>(render_pipeline);
    vkDestroyPipeline(m_device, vulkan_render_pipeline->pipeline, nullptr);
    delete vulkan_render_pipeline;
}

RS::CommandBuffer *VulkanRenderDriver::create_command_buffer()
{
    const VkCommandPoolCreateInfo command_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_queue_family,
    };

    VkCommandPool command_pool = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &command_pool), vkCreateCommandPool);
    HE_ASSERT(command_pool != VK_NULL_HANDLE);

    const VkCommandBufferAllocateInfo command_buffer_allocate_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer command_buffer = VK_NULL_HANDLE;
    HE_VK_CHECK(vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &command_buffer), vkAllocateCommandBuffers);
    HE_ASSERT(command_buffer != VK_NULL_HANDLE);

    constexpr VkFenceCreateInfo fence_create_info = {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkFence render_fence = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateFence(m_device, &fence_create_info, nullptr, &render_fence), vkCreateFence);
    HE_ASSERT(render_fence != VK_NULL_HANDLE);

    VkSemaphoreTypeCreateInfo submit_semaphore_type_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext = nullptr,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = 0,
    };

    const VkSemaphoreCreateInfo submit_semaphore_create_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &submit_semaphore_type_create_info,
        .flags = 0,
    };

    VkSemaphore submit_semaphore = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateSemaphore(m_device, &submit_semaphore_create_info, nullptr, &submit_semaphore), vkCreateSemaphore);
    HE_ASSERT(submit_semaphore != VK_NULL_HANDLE);

    return new VulkanCommandBuffer({
        .command_pool = command_pool,
        .command_buffer = command_buffer,
        .render_fence = render_fence,
        .submit_semaphore = submit_semaphore,
    });
}

void VulkanRenderDriver::destroy_command_buffer(RS::CommandBuffer *command_buffer)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);
    vkDestroyCommandPool(m_device, vulkan_command_buffer->command_pool, nullptr);
    vkDestroyFence(m_device, vulkan_command_buffer->render_fence, nullptr);
    vkDestroySemaphore(m_device, vulkan_command_buffer->submit_semaphore, nullptr);
    delete vulkan_command_buffer;
}

void VulkanRenderDriver::acquire_command_buffer(RS::CommandBuffer *command_buffer)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    const uint64_t wait_frame_index = vulkan_command_buffer->semaphore_counter;
    const VkSemaphoreWaitInfo semaphore_wait_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .pNext = nullptr,
        .flags = 0,
        .semaphoreCount = 1,
        .pSemaphores = &vulkan_command_buffer->submit_semaphore,
        .pValues = &wait_frame_index,
    };
    HE_VK_CHECK(vkWaitSemaphores(m_device, &semaphore_wait_info, std::numeric_limits<uint64_t>::max()), vkWaitSemaphores);

    HE_VK_CHECK(vkResetCommandBuffer(vulkan_command_buffer->command_buffer, 0), vkResetCommandBuffer);

    constexpr VkCommandBufferBeginInfo command_buffer_begin_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    HE_VK_CHECK(vkBeginCommandBuffer(vulkan_command_buffer->command_buffer, &command_buffer_begin_info), vkBeginCommandBuffer);
}

void VulkanRenderDriver::submit_command_buffer(RS::CommandBuffer *command_buffer)
{
    VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    // FIXME: Transition to Present
    if (vulkan_command_buffer->swapchain_texture_acquired)
    {
        RS::Texture *swapchain_texture = m_swapchain_textures[m_swapchain_texture_index];
        transition_texture_layout(command_buffer, swapchain_texture, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }

    HE_VK_CHECK(vkEndCommandBuffer(vulkan_command_buffer->command_buffer), vkEndCommandBuffer);

    if (vulkan_command_buffer->swapchain_texture_acquired)
    {
        HE_VK_CHECK(
            vkWaitForFences(m_device, 1, &vulkan_command_buffer->render_fence, VK_TRUE, std::numeric_limits<uint64_t>::max()), vkWaitForFences);
        HE_VK_CHECK(vkResetFences(m_device, 1, &vulkan_command_buffer->render_fence), vkResetFences);
    }

    vulkan_command_buffer->semaphore_counter += 1;

    const VkCommandBufferSubmitInfo command_buffer_submit_info = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext = nullptr,
        .commandBuffer = vulkan_command_buffer->command_buffer,
        .deviceMask = 0,
    };

    const VkSemaphoreSubmitInfo submit_semaphore_submit_info = {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext = nullptr,
        .semaphore = vulkan_command_buffer->submit_semaphore,
        .value = vulkan_command_buffer->semaphore_counter,
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .deviceIndex = 0,
    };

    const VkSubmitInfo2 submit_info = {
        .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
        .pNext = nullptr,
        .flags = 0,
        .waitSemaphoreInfoCount = 0,
        .pWaitSemaphoreInfos = nullptr,
        .commandBufferInfoCount = 1,
        .pCommandBufferInfos = &command_buffer_submit_info,
        .signalSemaphoreInfoCount = 1,
        .pSignalSemaphoreInfos = &submit_semaphore_submit_info,
    };

    HE_VK_CHECK(vkQueueSubmit2(m_queue, 1, &submit_info, VK_NULL_HANDLE), vkQueueSubmit2);
}

uint32_t VulkanRenderDriver::acquire_swapchain_texture(RS::CommandBuffer *command_buffer)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    /*
    destroy_resources();

    if (vulkan_surface.resized())
    {
        vulkan_surface.rebuild();
    }
    */

    uint32_t image_index = 0;
    HE_VK_CHECK(
        vkAcquireNextImageKHR(
            m_device, m_swapchain, std::numeric_limits<uint64_t>::max(), VK_NULL_HANDLE, vulkan_command_buffer->render_fence, &image_index),
        vkAcquireNextImageKHR);

    m_swapchain_texture_index = image_index;

    return m_swapchain_texture_index;
}

void VulkanRenderDriver::present()
{
    const VkPresentInfoKHR present_info = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 0,
        .pWaitSemaphores = nullptr,
        .swapchainCount = 1,
        .pSwapchains = &m_swapchain,
        .pImageIndices = &m_swapchain_texture_index,
        .pResults = nullptr,
    };

    HE_VK_CHECK(vkQueuePresentKHR(m_queue, &present_info), vkQueuePresentKHR);
}

void VulkanRenderDriver::begin_compute_pass(RS::CommandBuffer *command_buffer)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    // FIXME: Add Marker
}

void VulkanRenderDriver::end_compute_pass(RS::CommandBuffer *command_buffer)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    // FIXME: Add Marker
}

void VulkanRenderDriver::bind_compute_pipeline(RS::CommandBuffer *command_buffer, RS::ComputePipeline *pipeline)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);
    const VulkanComputePipeline *vulkan_compute_pipeline = reinterpret_cast<VulkanComputePipeline *>(pipeline);

    vkCmdBindPipeline(vulkan_command_buffer->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_compute_pipeline->pipeline);
}

void VulkanRenderDriver::begin_render_pass(RS::CommandBuffer *command_buffer, const RS::RenderPassDescriptor &descriptor, RS::Texture *texture)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);
    const VulkanTexture *vulkan_texture = reinterpret_cast<VulkanTexture *>(texture);

    // FIXME: Add Marker

    // FIXME: Should this always use the first image?
    const VkExtent2D render_area_extent = {
        .width = vulkan_texture->descriptor.width,
        .height = vulkan_texture->descriptor.height,
    };

    constexpr VkOffset2D render_area_offset = {
        .x = 0,
        .y = 0,
    };

    const VkRect2D render_area = {
        .offset = render_area_offset,
        .extent = render_area_extent,
    };

    constexpr VkClearValue clear_value = {
        .color =
            {
                .float32 =
                    {
                        0.0f,
                        0.0f,
                        0.0f,
                        1.0f,
                    },
            },
    };

    std::vector<VkRenderingAttachmentInfo> color_attachments = {};

    const VkRenderingAttachmentInfo color_attachment_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = vulkan_texture->image_view,
        .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        //.loadOp = VulkanRenderPass::get_attachment_load_operation(color_attachment.operation.load_operation),
        //.storeOp = VulkanRenderPass::get_attachment_store_operation(color_attachment.operation.store_operation),
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .clearValue = clear_value,
    };

    color_attachments.push_back(color_attachment_info);

    /*
    for (const ColorAttachment &color_attachment : m_color_attachments)
    {
        const VulkanTextureView &color_attachment_view = static_cast<const VulkanTextureView &>(*color_attachment.view);

        const VkRenderingAttachmentInfo color_attachment_info = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = color_attachment_view.image_view(),
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = VulkanRenderPass::get_attachment_load_operation(color_attachment.operation.load_operation),
            .storeOp = VulkanRenderPass::get_attachment_store_operation(color_attachment.operation.store_operation),
            .clearValue = clear_value,
        };

        color_attachments.push_back(color_attachment_info);
    }
    */

    constexpr VkClearValue depth_clear_value = {
        .depthStencil =
            {
                .depth = 1.0,
                .stencil = 0,
            },
    };

    /*
    const VkImageView depth_attachment_view = m_depth_stencil_attachment.view == nullptr
                                                  ? VK_NULL_HANDLE
                                                  : static_cast<const VulkanTextureView &>(*m_depth_stencil_attachment.view).image_view();


    const VkRenderingAttachmentInfo depth_attachment_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
        .pNext = nullptr,
        .imageView = depth_attachment_view,
        .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
        .resolveMode = VK_RESOLVE_MODE_NONE,
        .resolveImageView = VK_NULL_HANDLE,
        .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .loadOp = VulkanRenderPass::get_attachment_load_operation(m_depth_stencil_attachment.depth_operation.load_operation),
        .storeOp = VulkanRenderPass::get_attachment_store_operation(m_depth_stencil_attachment.depth_operation.store_operation),
        .clearValue = depth_clear_value,
    };
    */

    const VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = render_area,
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = static_cast<uint32_t>(color_attachments.size()),
        .pColorAttachments = color_attachments.data(),
        //.pDepthAttachment = depth_attachment_view == nullptr ? nullptr : &depth_attachment_info,
        .pDepthAttachment = nullptr,
        .pStencilAttachment = nullptr,
    };

    const VkViewport viewport = {
        .x = 0.0,
        .y = 0,
        .width = static_cast<float>(render_area_extent.width),
        .height = static_cast<float>(render_area_extent.height),
        .minDepth = 0.0,
        .maxDepth = 1.0,
    };

    constexpr VkOffset2D offset = {
        .x = 0,
        .y = 0,
    };

    const VkRect2D scissor = {
        .offset = offset,
        .extent = render_area_extent,
    };

    vkCmdBeginRendering(vulkan_command_buffer->command_buffer, &rendering_info);
    vkCmdSetViewport(vulkan_command_buffer->command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(vulkan_command_buffer->command_buffer, 0, 1, &scissor);
}

void VulkanRenderDriver::end_render_pass(RS::CommandBuffer *command_buffer)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    vkCmdEndRendering(vulkan_command_buffer->command_buffer);

    // FIXME: Add Marker
}

void VulkanRenderDriver::bind_render_pipeline(RS::CommandBuffer *command_buffer, RS::RenderPipeline *pipeline)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);
    const VulkanRenderPipeline *vulkan_render_pipeline = reinterpret_cast<VulkanRenderPipeline *>(pipeline);

    vkCmdBindPipeline(vulkan_command_buffer->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_render_pipeline->pipeline);
}

void VulkanRenderDriver::set_viewport(RS::CommandBuffer *command_buffer, const RS::Viewport &viewport)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    const VkViewport vk_viewport = {
        .x = viewport.x,
        .y = viewport.y,
        .width = viewport.width,
        .height = viewport.height,
        .minDepth = viewport.min_depth,
        .maxDepth = viewport.max_depth,
    };

    vkCmdSetViewport(vulkan_command_buffer->command_buffer, 0, 1, &vk_viewport);
}

void VulkanRenderDriver::set_scissor(RS::CommandBuffer *command_buffer, const RS::Scissor &scissor)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    const VkOffset2D offset = {
        .x = scissor.x,
        .y = scissor.y,
    };

    const VkExtent2D extent = {
        .width = scissor.width,
        .height = scissor.height,
    };

    const VkRect2D vk_scissor = {
        .offset = offset,
        .extent = extent,
    };

    vkCmdSetScissor(vulkan_command_buffer->command_buffer, 0, 1, &vk_scissor);
}

void VulkanRenderDriver::draw(
    RS::CommandBuffer *command_buffer,
    const uint32_t vertex_count,
    const uint32_t instance_count,
    const uint32_t first_vertex,
    const uint32_t first_instance)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    vkCmdDraw(vulkan_command_buffer->command_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

void VulkanRenderDriver::create_instance()
{
    constexpr VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
        .pUserData = nullptr,
    };

    const void *next = m_validation_layer_enabled ? &debug_create_info : nullptr;

    constexpr VkApplicationInfo application_info = {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "HyperEngine",
        .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .pEngineName = "HyperEngine",
        .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    const uint32_t layer_count = m_validation_layer_enabled ? 1 : 0;
    const char *const *layers = m_validation_layer_enabled ? &s_validation_layer : nullptr;

    uint32_t required_extension_count = 0;
    const char *const *required_extensions = SDL_Vulkan_GetInstanceExtensions(&required_extension_count);

    std::vector<const char *> extensions(required_extensions, required_extensions + required_extension_count);
    if (m_validation_layer_enabled)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    const VkInstanceCreateInfo instance_create_info = {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = next,
        .flags = 0,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = layer_count,
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
    };

    const VkResult result = vkCreateInstance(&instance_create_info, nullptr, &m_instance);
    HE_VK_CHECK(result, vkCreateInstance);

    volkLoadInstance(m_instance);
}

void VulkanRenderDriver::create_debug_messenger()
{
    constexpr VkDebugUtilsMessengerCreateInfoEXT debug_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
        .pUserData = nullptr,
    };

    const VkResult result = vkCreateDebugUtilsMessengerEXT(m_instance, &debug_create_info, nullptr, &m_debug_messenger);
    HE_VK_CHECK(result, vkCreateDebugUtilsMessengerEXT);
}

void VulkanRenderDriver::choose_physical_device()
{
    VkResult result = VK_SUCCESS;

    uint32_t device_count = 0;
    result = vkEnumeratePhysicalDevices(m_instance, &device_count, nullptr);
    HE_VK_CHECK(result, vkEnumeratePhysicalDevices);

    std::vector<VkPhysicalDevice> physical_devices(device_count);
    result = vkEnumeratePhysicalDevices(m_instance, &device_count, physical_devices.data());
    HE_VK_CHECK(result, vkEnumeratePhysicalDevices);

    std::multimap<uint32_t, VkPhysicalDevice> possible_physical_devices = {};
    for (const VkPhysicalDevice &physical_device : physical_devices)
    {
        const uint32_t score = rate_physical_device(physical_device);
        possible_physical_devices.insert({score, physical_device});
    }

    HE_ASSERT(possible_physical_devices.rbegin()->first != 0);

    m_physical_device = possible_physical_devices.rbegin()->second;

    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(m_physical_device, &properties);

    const std::string_view device_type = [&properties]()
    {
        switch (properties.deviceType)
        {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "Other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "Virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "CPU";
        default:
            HE_UNREACHABLE();
        }
    }();

    HE_INFO("Physical Device Info:");
    HE_INFO("  Name: {}", properties.deviceName);
    HE_INFO(
        "  API Version: {}.{}.{}",
        VK_VERSION_MAJOR(properties.apiVersion),
        VK_VERSION_MINOR(properties.apiVersion),
        VK_VERSION_PATCH(properties.apiVersion));
    HE_INFO("  Type: {}", device_type);
}

uint32_t VulkanRenderDriver::rate_physical_device(const VkPhysicalDevice &physical_device) const
{
    uint32_t score = 0;

    const std::optional<uint32_t> queue_family = find_queue_family(physical_device);
    if (!queue_family.has_value())
    {
        return 0;
    }

    const bool extensions_supported = check_extension_support(physical_device);
    if (!extensions_supported)
    {
        return 0;
    }

    const bool features_supported = check_feature_support(physical_device);
    if (!features_supported)
    {
        return 0;
    }

    VkPhysicalDeviceProperties device_properties = {};
    vkGetPhysicalDeviceProperties(physical_device, &device_properties);

    switch (device_properties.deviceType)
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:
        score += 0;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
        score += 500;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
        score += 1000;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
        score += 250;
        break;
    case VK_PHYSICAL_DEVICE_TYPE_CPU:
        score += 100;
        break;
    default:
        HE_UNREACHABLE();
    }

    return score;
}

std::optional<uint32_t> VulkanRenderDriver::find_queue_family(const VkPhysicalDevice &physical_device) const
{
    uint32_t queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

    uint32_t index = 0;
    for (const VkQueueFamilyProperties &queue_family : queue_families)
    {
        const bool graphics_supported = queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        const bool present_supported = SDL_Vulkan_GetPresentationSupport(m_instance, physical_device, index);

        if (graphics_supported && present_supported)
        {
            return index;
        }

        ++index;
    }

    return std::nullopt;
}

void VulkanRenderDriver::create_device()
{
    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
        .pNext = nullptr,
        .dynamicRendering = VK_TRUE,
    };

    VkPhysicalDeviceTimelineSemaphoreFeatures timeline_semaphore = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
        .pNext = &dynamic_rendering,
        .timelineSemaphore = VK_TRUE,
    };

    VkPhysicalDeviceSynchronization2Features synchronization2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
        .pNext = &timeline_semaphore,
        .synchronization2 = VK_TRUE,
    };

    VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
        .pNext = &synchronization2,
        .shaderInputAttachmentArrayDynamicIndexing = VK_TRUE,
        .shaderUniformTexelBufferArrayDynamicIndexing = VK_TRUE,
        .shaderStorageTexelBufferArrayDynamicIndexing = VK_TRUE,
        .shaderUniformBufferArrayNonUniformIndexing = VK_TRUE,
        .shaderSampledImageArrayNonUniformIndexing = VK_TRUE,
        .shaderStorageBufferArrayNonUniformIndexing = VK_TRUE,
        .shaderStorageImageArrayNonUniformIndexing = VK_TRUE,
        .shaderInputAttachmentArrayNonUniformIndexing = VK_TRUE,
        .shaderUniformTexelBufferArrayNonUniformIndexing = VK_TRUE,
        .shaderStorageTexelBufferArrayNonUniformIndexing = VK_TRUE,
        .descriptorBindingUniformBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingSampledImageUpdateAfterBind = VK_TRUE,
        .descriptorBindingStorageImageUpdateAfterBind = VK_TRUE,
        .descriptorBindingStorageBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingUniformTexelBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingStorageTexelBufferUpdateAfterBind = VK_TRUE,
        .descriptorBindingUpdateUnusedWhilePending = VK_TRUE,
        .descriptorBindingPartiallyBound = VK_TRUE,
        .descriptorBindingVariableDescriptorCount = VK_TRUE,
        .runtimeDescriptorArray = VK_TRUE,
    };

    VkPhysicalDeviceFeatures2 device_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &descriptor_indexing,
        .features = {},
    };

    const std::optional<uint32_t> queue_family = find_queue_family(m_physical_device);

    constexpr float queue_priority = 1.0f;
    const VkDeviceQueueCreateInfo queue_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = queue_family.value(),
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    const uint32_t layer_count = m_validation_layer_enabled ? 1 : 0;
    const char *const *layers = m_validation_layer_enabled ? &s_validation_layer : nullptr;

    const VkDeviceCreateInfo device_create_info = {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &device_features,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
        .enabledLayerCount = layer_count,
        .ppEnabledLayerNames = layers,
        .enabledExtensionCount = static_cast<uint32_t>(s_device_extensions.size()),
        .ppEnabledExtensionNames = s_device_extensions.data(),
        .pEnabledFeatures = nullptr,
    };

    const VkResult result = vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device);
    HE_VK_CHECK(result, vkCreateDevice);

    volkLoadDevice(m_device);

    VkQueue queue = VK_NULL_HANDLE;
    vkGetDeviceQueue(m_device, queue_family.value(), 0, &queue);

    m_queue_family = queue_family.value();
    m_queue = queue;
}

void VulkanRenderDriver::create_allocator()
{
    const VmaVulkanFunctions functions = {
        .vkGetInstanceProcAddr = vkGetInstanceProcAddr,
        .vkGetDeviceProcAddr = vkGetDeviceProcAddr,
        .vkGetPhysicalDeviceProperties = vkGetPhysicalDeviceProperties,
        .vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties,
        .vkAllocateMemory = vkAllocateMemory,
        .vkFreeMemory = vkFreeMemory,
        .vkMapMemory = vkMapMemory,
        .vkUnmapMemory = vkUnmapMemory,
        .vkFlushMappedMemoryRanges = vkFlushMappedMemoryRanges,
        .vkInvalidateMappedMemoryRanges = vkInvalidateMappedMemoryRanges,
        .vkBindBufferMemory = vkBindBufferMemory,
        .vkBindImageMemory = vkBindImageMemory,
        .vkGetBufferMemoryRequirements = vkGetBufferMemoryRequirements,
        .vkGetImageMemoryRequirements = vkGetImageMemoryRequirements,
        .vkCreateBuffer = vkCreateBuffer,
        .vkDestroyBuffer = vkDestroyBuffer,
        .vkCreateImage = vkCreateImage,
        .vkDestroyImage = vkDestroyImage,
        .vkCmdCopyBuffer = vkCmdCopyBuffer,
        .vkGetBufferMemoryRequirements2KHR = vkGetBufferMemoryRequirements2KHR,
        .vkGetImageMemoryRequirements2KHR = vkGetImageMemoryRequirements2KHR,
        .vkBindBufferMemory2KHR = vkBindBufferMemory2KHR,
        .vkBindImageMemory2KHR = vkBindImageMemory2KHR,
        .vkGetPhysicalDeviceMemoryProperties2KHR = vkGetPhysicalDeviceMemoryProperties2KHR,
        .vkGetDeviceBufferMemoryRequirements = vkGetDeviceBufferMemoryRequirements,
        .vkGetDeviceImageMemoryRequirements = vkGetDeviceImageMemoryRequirements,
    };

    const VmaAllocatorCreateInfo allocator_create_info = {
        .flags = 0,
        .physicalDevice = m_physical_device,
        .device = m_device,
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = &functions,
        .instance = m_instance,
        .vulkanApiVersion = VK_API_VERSION_1_3,
        .pTypeExternalMemoryHandleTypes = nullptr,
    };

    const VkResult result = vmaCreateAllocator(&allocator_create_info, &m_allocator);
    HE_VK_CHECK(result, vmaCreateAllocator);
}

void VulkanRenderDriver::create_surface(const WS &window_system, const WS::WindowId id)
{
    SDL_Window *window = window_system.get_native_window(id);

    HE_ASSERT(SDL_Vulkan_CreateSurface(window, m_instance, nullptr, &m_surface));
    HE_ASSERT(m_surface != VK_NULL_HANDLE);
}

void VulkanRenderDriver::create_swapchain(const WS &window_system, const WS::WindowId id)
{
    const glm::uvec2 size = window_system.get_window_size(id);

    VkSurfaceCapabilitiesKHR surface_capabilities = {};
    HE_VK_CHECK(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &surface_capabilities),
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR);

    const VkExtent2D surface_extent = choose_extent(size.x, size.y, surface_capabilities);

    uint32_t format_count = 0;
    HE_VK_CHECK(
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, nullptr), vkGetPhysicalDeviceSurfaceFormatsKHR);

    std::vector<VkSurfaceFormatKHR> formats(format_count);
    HE_VK_CHECK(
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physical_device, m_surface, &format_count, formats.data()), vkGetPhysicalDeviceSurfaceFormatsKHR);

    const VkSurfaceFormatKHR surface_format = choose_format(formats);
    m_swapchain_format = surface_format.format;

    uint32_t present_mode_count = 0;
    HE_VK_CHECK(
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &present_mode_count, nullptr),
        vkGetPhysicalDeviceSurfacePresentModesKHR);

    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    HE_VK_CHECK(
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physical_device, m_surface, &present_mode_count, present_modes.data()),
        vkGetPhysicalDeviceSurfacePresentModesKHR);

    const VkPresentModeKHR surface_present_mode = choose_present_mode(present_modes);

    m_min_image_count = surface_capabilities.minImageCount + 1;
    m_image_count = m_min_image_count;
    if (surface_capabilities.maxImageCount > 0 && m_image_count > surface_capabilities.maxImageCount)
    {
        m_image_count = surface_capabilities.maxImageCount;
    }

    const VkSwapchainCreateInfoKHR swapchain_create_info = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = m_surface,
        .minImageCount = m_image_count,
        .imageFormat = m_swapchain_format,
        .imageColorSpace = surface_format.colorSpace,
        .imageExtent = surface_extent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = surface_capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = surface_present_mode,
        .clipped = true,
        .oldSwapchain = VK_NULL_HANDLE,
    };

    HE_VK_CHECK(vkCreateSwapchainKHR(m_device, &swapchain_create_info, nullptr, &m_swapchain), vkCreateSwapchainKHR);
    HE_ASSERT(m_swapchain != VK_NULL_HANDLE);

    uint32_t image_count = 0;
    HE_VK_CHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr), vkGetSwapchainImagesKHR);

    std::vector<VkImage> images(image_count);
    HE_VK_CHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, images.data()), vkGetSwapchainImagesKHR);

    for (const VkImage image : images)
    {
        // FIXME: Go through create texture function

        const RS::TextureDescriptor descriptor = {
            .label = std::nullopt,
            .width = surface_extent.width,
            .height = surface_extent.height,
            .depth = 1,
            .array_size = 1,
            .mip_levels = 1,
            .format = format_to_texture_format(surface_format.format),
            .dimension = RS::Dimension::Texture2D,
            .usage = RS::TextureUsage::RenderAttachment,
        };

        const VkImageViewType view_type = get_image_view_type(descriptor.dimension);

        const VkFormat format = get_format(descriptor.format);

        constexpr VkComponentMapping component_mapping = {
            .r = VK_COMPONENT_SWIZZLE_IDENTITY,
            .g = VK_COMPONENT_SWIZZLE_IDENTITY,
            .b = VK_COMPONENT_SWIZZLE_IDENTITY,
            .a = VK_COMPONENT_SWIZZLE_IDENTITY,
        };

        const VkImageAspectFlags aspect_mask = get_image_aspect_flags(descriptor.format);
        const VkImageSubresourceRange subresource_range = {
            .aspectMask = aspect_mask,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        const VkImageViewCreateInfo image_view_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = view_type,
            .format = format,
            .components = component_mapping,
            .subresourceRange = subresource_range,
        };

        VkImageView image_view = VK_NULL_HANDLE;
        HE_VK_CHECK(vkCreateImageView(m_device, &image_view_create_info, nullptr, &image_view), vkCreateImageView);
        HE_ASSERT(image_view != VK_NULL_HANDLE);

        RS::Texture *texture = new VulkanTexture({
            .image = image,
            .allocation = VK_NULL_HANDLE,
            .image_view = image_view,
        });

        texture->descriptor = descriptor;

        m_swapchain_textures.push_back(texture);
    }
}

RS::Buffer *VulkanRenderDriver::create_internal_buffer(
    const std::optional<std::string> &label,
    const uint64_t byte_size,
    const BitFlags<RS::BufferUsage> usage,
    const bool staging) const
{
    const VkBufferUsageFlags usage_flags = get_buffer_usage_flags(usage);

    uint64_t buffer_byte_size = byte_size;
    if (buffer_byte_size < 65535)
    {
        buffer_byte_size = (buffer_byte_size + 3) & ~3ull;
    }

    const VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = byte_size,
        .usage = usage_flags,
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    VmaAllocationCreateFlags allocation_flags = 0;
    if (staging)
    {
        allocation_flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        allocation_flags |= VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }

    const VmaAllocationCreateInfo allocation_create_info = {
        .flags = allocation_flags,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 0,
    };

    VkBuffer buffer = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    HE_VK_CHECK(vmaCreateBuffer(m_allocator, &buffer_create_info, &allocation_create_info, &buffer, &allocation, nullptr), vmaCreateBuffer);

    HE_ASSERT(buffer != VK_NULL_HANDLE);
    HE_ASSERT(allocation != VK_NULL_HANDLE);

    return new VulkanBuffer({
        .buffer = buffer,
        .allocation = allocation,
    });
}

void VulkanRenderDriver::transition_texture_layout(RS::CommandBuffer *command_buffer, RS::Texture *texture, const VkImageLayout new_layout)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);
    VulkanTexture *vulkan_texture = reinterpret_cast<VulkanTexture *>(texture);

    // FIXME: Add more specific src/dst stage/access masks

    // FIXME: Add Subresources
    const VkImageSubresourceRange subresource_range = {
        .aspectMask = get_image_aspect_flags(vulkan_texture->descriptor.format),
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const VkImageMemoryBarrier2 image_memory_barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
        .oldLayout = vulkan_texture->image_layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = 0,
        .dstQueueFamilyIndex = 0,
        .image = vulkan_texture->image,
        .subresourceRange = subresource_range,
    };

    const VkDependencyInfo dependency_info = {
        .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
        .pNext = nullptr,
        .dependencyFlags = 0,
        .memoryBarrierCount = 0,
        .pMemoryBarriers = nullptr,
        .bufferMemoryBarrierCount = 0,
        .pBufferMemoryBarriers = nullptr,
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &image_memory_barrier,
    };

    vkCmdPipelineBarrier2(vulkan_command_buffer->command_buffer, &dependency_info);

    vulkan_texture->image_layout = new_layout;
}

bool VulkanRenderDriver::is_validation_layer_supported()
{
    VkResult result = VK_SUCCESS;

    uint32_t layer_count = 0;
    result = vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
    HE_VK_CHECK(result, vkEnumerateInstanceLayerProperties);

    std::vector<VkLayerProperties> layer_properties(layer_count);
    result = vkEnumerateInstanceLayerProperties(&layer_count, layer_properties.data());
    HE_VK_CHECK(result, vkEnumerateInstanceLayerProperties);

    const bool validation_layer_supported = std::ranges::any_of(
        layer_properties,
        [&](const VkLayerProperties &properties)
        {
            return std::strcmp(properties.layerName, s_validation_layer);
        });

    return validation_layer_supported;
}

bool VulkanRenderDriver::check_extension_support(const VkPhysicalDevice &physical_device)
{
    VkResult result = VK_SUCCESS;

    uint32_t extension_count = 0;
    result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);
    HE_VK_CHECK(result, vkEnumerateDeviceExtensionProperties);

    std::vector<VkExtensionProperties> extensions(extension_count);
    result = vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions.data());
    HE_VK_CHECK(result, vkEnumerateDeviceExtensionProperties);

    std::set<std::string> required_extensions(s_device_extensions.begin(), s_device_extensions.end());
    for (const VkExtensionProperties &extension : extensions)
    {
        required_extensions.erase(extension.extensionName);
    }

    return required_extensions.empty();
}

bool VulkanRenderDriver::check_feature_support(const VkPhysicalDevice &physical_device)
{
    VkPhysicalDeviceDynamicRenderingFeatures dynamic_rendering = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES,
        .pNext = nullptr,
        .dynamicRendering = VK_FALSE,
    };

    VkPhysicalDeviceTimelineSemaphoreFeatures timeline_semaphore = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES,
        .pNext = &dynamic_rendering,
        .timelineSemaphore = VK_FALSE,
    };

    VkPhysicalDeviceSynchronization2Features synchronization2 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SYNCHRONIZATION_2_FEATURES,
        .pNext = &timeline_semaphore,
        .synchronization2 = VK_FALSE,
    };

    VkPhysicalDeviceDescriptorIndexingFeatures descriptor_indexing = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES,
        .pNext = &synchronization2,
        .shaderInputAttachmentArrayDynamicIndexing = VK_FALSE,
        .shaderUniformTexelBufferArrayDynamicIndexing = VK_FALSE,
        .shaderStorageTexelBufferArrayDynamicIndexing = VK_FALSE,
        .shaderUniformBufferArrayNonUniformIndexing = VK_FALSE,
        .shaderSampledImageArrayNonUniformIndexing = VK_FALSE,
        .shaderStorageBufferArrayNonUniformIndexing = VK_FALSE,
        .shaderStorageImageArrayNonUniformIndexing = VK_FALSE,
        .shaderInputAttachmentArrayNonUniformIndexing = VK_FALSE,
        .shaderUniformTexelBufferArrayNonUniformIndexing = VK_FALSE,
        .shaderStorageTexelBufferArrayNonUniformIndexing = VK_FALSE,
        .descriptorBindingUniformBufferUpdateAfterBind = VK_FALSE,
        .descriptorBindingSampledImageUpdateAfterBind = VK_FALSE,
        .descriptorBindingStorageImageUpdateAfterBind = VK_FALSE,
        .descriptorBindingStorageBufferUpdateAfterBind = VK_FALSE,
        .descriptorBindingUniformTexelBufferUpdateAfterBind = VK_FALSE,
        .descriptorBindingStorageTexelBufferUpdateAfterBind = VK_FALSE,
        .descriptorBindingUpdateUnusedWhilePending = VK_FALSE,
        .descriptorBindingPartiallyBound = VK_FALSE,
        .descriptorBindingVariableDescriptorCount = VK_FALSE,
        .runtimeDescriptorArray = VK_FALSE,
    };

    VkPhysicalDeviceFeatures2 device_features = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &descriptor_indexing,
        .features = {},
    };
    vkGetPhysicalDeviceFeatures2(physical_device, &device_features);

    const bool dynamic_rendering_supported = dynamic_rendering.dynamicRendering;
    const bool timeline_semaphore_supported = timeline_semaphore.timelineSemaphore;
    const bool synchronization2_supported = synchronization2.synchronization2;
    const bool descriptor_indexing_supported =
        descriptor_indexing.shaderUniformBufferArrayNonUniformIndexing & descriptor_indexing.shaderSampledImageArrayNonUniformIndexing &
        descriptor_indexing.shaderStorageBufferArrayNonUniformIndexing & descriptor_indexing.shaderStorageImageArrayNonUniformIndexing &
        descriptor_indexing.descriptorBindingUniformBufferUpdateAfterBind & descriptor_indexing.descriptorBindingSampledImageUpdateAfterBind &
        descriptor_indexing.descriptorBindingStorageImageUpdateAfterBind & descriptor_indexing.descriptorBindingStorageBufferUpdateAfterBind &
        descriptor_indexing.descriptorBindingUpdateUnusedWhilePending & descriptor_indexing.descriptorBindingPartiallyBound &
        descriptor_indexing.descriptorBindingVariableDescriptorCount & descriptor_indexing.runtimeDescriptorArray;

    const bool features_supported =
        dynamic_rendering_supported & timeline_semaphore_supported & synchronization2_supported & descriptor_indexing_supported;

    return features_supported;
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderDriver::debug_callback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    const VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *)
{
    switch (message_severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        HE_TRACE("{}", callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        HE_INFO("{}", callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        HE_WARN("{}", callback_data->pMessage);
        break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        HE_ERROR("{}", callback_data->pMessage);
        break;
    default:
        break;
    }

    return VK_FALSE;
}

VkExtent2D VulkanRenderDriver::choose_extent(const uint32_t width, const uint32_t height, const VkSurfaceCapabilitiesKHR &capabilities)
{
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max() ||
        capabilities.currentExtent.height != std::numeric_limits<uint32_t>::max())
    {
        return capabilities.currentExtent;
    }

    const VkExtent2D extent = {
        .width = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
        .height = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height),
    };

    return extent;
}

VkSurfaceFormatKHR VulkanRenderDriver::choose_format(const std::vector<VkSurfaceFormatKHR> &formats)
{
    for (const VkSurfaceFormatKHR &format : formats)
    {
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return format;
        }
    }

    return formats[0];
}

VkPresentModeKHR VulkanRenderDriver::choose_present_mode(const std::vector<VkPresentModeKHR> &present_modes)
{
    for (const VkPresentModeKHR &present_mode : present_modes)
    {
        if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return present_mode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkBufferUsageFlags VulkanRenderDriver::get_buffer_usage_flags(const BitFlags<RS::BufferUsage> buffer_usage_flags)
{
    VkBufferUsageFlags usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if (buffer_usage_flags & RS::BufferUsage::Index)
    {
        usage_flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }

    if (buffer_usage_flags & RS::BufferUsage::Indirect)
    {
        usage_flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    }

    if (buffer_usage_flags & RS::BufferUsage::Storage)
    {
        usage_flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    return usage_flags;
}

VkFilter VulkanRenderDriver::get_filter(const RS::Filter filter)
{
    switch (filter)
    {
    case RS::Filter::Nearest:
        return VK_FILTER_NEAREST;
    case RS::Filter::Linear:
        return VK_FILTER_LINEAR;
    default:
        HE_UNREACHABLE();
    }
}

VkSamplerMipmapMode VulkanRenderDriver::get_sampler_mipmap_mode(const RS::Filter filter)
{
    switch (filter)
    {
    case RS::Filter::Nearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case RS::Filter::Linear:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    default:
        HE_UNREACHABLE();
    }
}

VkSamplerAddressMode VulkanRenderDriver::get_sampler_address_mode(const RS::AddressMode filter)
{
    switch (filter)
    {
    case RS::AddressMode::Repeat:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case RS::AddressMode::MirroredRepeat:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case RS::AddressMode::ClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case RS::AddressMode::ClampToBorder:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case RS::AddressMode::MirrorClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    default:
        HE_UNREACHABLE();
    }
}

VkBorderColor VulkanRenderDriver::get_border_color(const RS::BorderColor border_color)
{
    switch (border_color)
    {
    case RS::BorderColor::TransparentBlack:
        return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    case RS::BorderColor::OpaqueBlack:
        return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    case RS::BorderColor::OpaqueWhite:
        return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    default:
        HE_UNREACHABLE();
    }
}

RS::Format VulkanRenderDriver::format_to_texture_format(const VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_UNDEFINED:
        return RS::Format::Unknown;
    case VK_FORMAT_R8_UNORM:
        return RS::Format::R8Unorm;
    case VK_FORMAT_R8_SNORM:
        return RS::Format::R8Snorm;
    case VK_FORMAT_R8_UINT:
        return RS::Format::R8Uint;
    case VK_FORMAT_R8_SINT:
        return RS::Format::R8Sint;
    case VK_FORMAT_R8_SRGB:
        return RS::Format::R8Srgb;
    case VK_FORMAT_R8G8_UNORM:
        return RS::Format::Rg8Unorm;
    case VK_FORMAT_R8G8_SNORM:
        return RS::Format::Rg8Snorm;
    case VK_FORMAT_R8G8_UINT:
        return RS::Format::Rg8Uint;
    case VK_FORMAT_R8G8_SINT:
        return RS::Format::Rg8Sint;
    case VK_FORMAT_R8G8_SRGB:
        return RS::Format::Rg8Srgb;
    case VK_FORMAT_R8G8B8_UNORM:
        return RS::Format::Rgb8Unorm;
    case VK_FORMAT_R8G8B8_SNORM:
        return RS::Format::Rgb8Snorm;
    case VK_FORMAT_R8G8B8_UINT:
        return RS::Format::Rgb8Uint;
    case VK_FORMAT_R8G8B8_SINT:
        return RS::Format::Rgb8Sint;
    case VK_FORMAT_R8G8B8_SRGB:
        return RS::Format::Rgb8Srgb;
    case VK_FORMAT_B8G8R8_UNORM:
        return RS::Format::Bgr8Unorm;
    case VK_FORMAT_B8G8R8_SNORM:
        return RS::Format::Bgr8Snorm;
    case VK_FORMAT_B8G8R8_UINT:
        return RS::Format::Bgr8Uint;
    case VK_FORMAT_B8G8R8_SINT:
        return RS::Format::Bgr8Sint;
    case VK_FORMAT_B8G8R8_SRGB:
        return RS::Format::Bgr8Srgb;
    case VK_FORMAT_R8G8B8A8_UNORM:
        return RS::Format::Rgba8Unorm;
    case VK_FORMAT_R8G8B8A8_SNORM:
        return RS::Format::Rgba8Snorm;
    case VK_FORMAT_R8G8B8A8_UINT:
        return RS::Format::Rgba8Uint;
    case VK_FORMAT_R8G8B8A8_SINT:
        return RS::Format::Rgba8Sint;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return RS::Format::Rgba8Srgb;
    case VK_FORMAT_B8G8R8A8_UNORM:
        return RS::Format::Bgra8Unorm;
    case VK_FORMAT_B8G8R8A8_SNORM:
        return RS::Format::Bgra8Snorm;
    case VK_FORMAT_B8G8R8A8_UINT:
        return RS::Format::Bgra8Uint;
    case VK_FORMAT_B8G8R8A8_SINT:
        return RS::Format::Bgra8Sint;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return RS::Format::Bgra8Srgb;
    case VK_FORMAT_R16_UNORM:
        return RS::Format::R16Unorm;
    case VK_FORMAT_R16_SNORM:
        return RS::Format::R16Snorm;
    case VK_FORMAT_R16_UINT:
        return RS::Format::R16Uint;
    case VK_FORMAT_R16_SINT:
        return RS::Format::R16Sint;
    case VK_FORMAT_R16_SFLOAT:
        return RS::Format::R16Sfloat;
    case VK_FORMAT_R16G16_UNORM:
        return RS::Format::Rg16Unorm;
    case VK_FORMAT_R16G16_SNORM:
        return RS::Format::Rg16Snorm;
    case VK_FORMAT_R16G16_UINT:
        return RS::Format::Rg16Uint;
    case VK_FORMAT_R16G16_SINT:
        return RS::Format::Rg16Sint;
    case VK_FORMAT_R16G16_SFLOAT:
        return RS::Format::Rg16Sfloat;
    case VK_FORMAT_R16G16B16_UNORM:
        return RS::Format::Rgb16Unorm;
    case VK_FORMAT_R16G16B16_SNORM:
        return RS::Format::Rgb16Snorm;
    case VK_FORMAT_R16G16B16_UINT:
        return RS::Format::Rgb16Uint;
    case VK_FORMAT_R16G16B16_SINT:
        return RS::Format::Rgb16Sint;
    case VK_FORMAT_R16G16B16_SFLOAT:
        return RS::Format::Rgb16Sfloat;
    case VK_FORMAT_R16G16B16A16_UNORM:
        return RS::Format::Rgba16Unorm;
    case VK_FORMAT_R16G16B16A16_SNORM:
        return RS::Format::Rgba16Snorm;
    case VK_FORMAT_R16G16B16A16_UINT:
        return RS::Format::Rgba16Uint;
    case VK_FORMAT_R16G16B16A16_SINT:
        return RS::Format::Rgba16Sint;
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return RS::Format::Rgba16Sfloat;
    case VK_FORMAT_R32_UINT:
        return RS::Format::R32Uint;
    case VK_FORMAT_R32_SINT:
        return RS::Format::R32Sint;
    case VK_FORMAT_R32_SFLOAT:
        return RS::Format::R32Sfloat;
    case VK_FORMAT_R32G32_UINT:
        return RS::Format::Rg32Uint;
    case VK_FORMAT_R32G32_SINT:
        return RS::Format::Rg32Sint;
    case VK_FORMAT_R32G32_SFLOAT:
        return RS::Format::Rg32Sfloat;
    case VK_FORMAT_R32G32B32_UINT:
        return RS::Format::Rgb32Uint;
    case VK_FORMAT_R32G32B32_SINT:
        return RS::Format::Rgb32Sint;
    case VK_FORMAT_R32G32B32_SFLOAT:
        return RS::Format::Rgb32Sfloat;
    case VK_FORMAT_R32G32B32A32_UINT:
        return RS::Format::Rgba32Uint;
    case VK_FORMAT_R32G32B32A32_SINT:
        return RS::Format::Rgba32Sint;
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return RS::Format::Rgba32Sfloat;
    case VK_FORMAT_R64_UINT:
        return RS::Format::R64Uint;
    case VK_FORMAT_R64_SINT:
        return RS::Format::R64Sint;
    case VK_FORMAT_R64_SFLOAT:
        return RS::Format::R64Sfloat;
    case VK_FORMAT_R64G64_UINT:
        return RS::Format::Rg64Uint;
    case VK_FORMAT_R64G64_SINT:
        return RS::Format::Rg64Sint;
    case VK_FORMAT_R64G64_SFLOAT:
        return RS::Format::Rg64Sfloat;
    case VK_FORMAT_R64G64B64_UINT:
        return RS::Format::Rgb64Uint;
    case VK_FORMAT_R64G64B64_SINT:
        return RS::Format::Rgb64Sint;
    case VK_FORMAT_R64G64B64_SFLOAT:
        return RS::Format::Rgb64Sfloat;
    case VK_FORMAT_R64G64B64A64_UINT:
        return RS::Format::Rgba64Uint;
    case VK_FORMAT_R64G64B64A64_SINT:
        return RS::Format::Rgba64Sint;
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return RS::Format::Rgba64Sfloat;
    case VK_FORMAT_D16_UNORM:
        return RS::Format::D16Unorm;
    case VK_FORMAT_D32_SFLOAT:
        return RS::Format::D32Sfloat;
    case VK_FORMAT_S8_UINT:
        return RS::Format::S8Uint;
    case VK_FORMAT_D16_UNORM_S8_UINT:
        return RS::Format::D16UnormS8Uint;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return RS::Format::D24UnormS8Uint;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return RS::Format::D32SfloatS8Uint;
    default:
        HE_UNREACHABLE();
    }
}

VkFormat VulkanRenderDriver::get_format(const RS::Format format)
{
    switch (format)
    {
    case RS::Format::Unknown:
        return VK_FORMAT_UNDEFINED;
    case RS::Format::R8Unorm:
        return VK_FORMAT_R8_UNORM;
    case RS::Format::R8Snorm:
        return VK_FORMAT_R8_SNORM;
    case RS::Format::R8Uint:
        return VK_FORMAT_R8_UINT;
    case RS::Format::R8Sint:
        return VK_FORMAT_R8_SINT;
    case RS::Format::R8Srgb:
        return VK_FORMAT_R8_SRGB;
    case RS::Format::Rg8Unorm:
        return VK_FORMAT_R8G8_UNORM;
    case RS::Format::Rg8Snorm:
        return VK_FORMAT_R8G8_SNORM;
    case RS::Format::Rg8Uint:
        return VK_FORMAT_R8G8_UINT;
    case RS::Format::Rg8Sint:
        return VK_FORMAT_R8G8_SINT;
    case RS::Format::Rg8Srgb:
        return VK_FORMAT_R8G8_SRGB;
    case RS::Format::Rgb8Unorm:
        return VK_FORMAT_R8G8B8_UNORM;
    case RS::Format::Rgb8Snorm:
        return VK_FORMAT_R8G8B8_SNORM;
    case RS::Format::Rgb8Uint:
        return VK_FORMAT_R8G8B8_UINT;
    case RS::Format::Rgb8Sint:
        return VK_FORMAT_R8G8B8_SINT;
    case RS::Format::Rgb8Srgb:
        return VK_FORMAT_R8G8B8_SRGB;
    case RS::Format::Bgr8Unorm:
        return VK_FORMAT_B8G8R8_UNORM;
    case RS::Format::Bgr8Snorm:
        return VK_FORMAT_B8G8R8_SNORM;
    case RS::Format::Bgr8Uint:
        return VK_FORMAT_B8G8R8_UINT;
    case RS::Format::Bgr8Sint:
        return VK_FORMAT_B8G8R8_SINT;
    case RS::Format::Bgr8Srgb:
        return VK_FORMAT_B8G8R8_SRGB;
    case RS::Format::Rgba8Unorm:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case RS::Format::Rgba8Snorm:
        return VK_FORMAT_R8G8B8A8_SNORM;
    case RS::Format::Rgba8Uint:
        return VK_FORMAT_R8G8B8A8_UINT;
    case RS::Format::Rgba8Sint:
        return VK_FORMAT_R8G8B8A8_SINT;
    case RS::Format::Rgba8Srgb:
        return VK_FORMAT_R8G8B8A8_SRGB;
    case RS::Format::Bgra8Unorm:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case RS::Format::Bgra8Snorm:
        return VK_FORMAT_B8G8R8A8_SNORM;
    case RS::Format::Bgra8Uint:
        return VK_FORMAT_B8G8R8A8_UINT;
    case RS::Format::Bgra8Sint:
        return VK_FORMAT_B8G8R8A8_SINT;
    case RS::Format::Bgra8Srgb:
        return VK_FORMAT_B8G8R8A8_SRGB;
    case RS::Format::R16Unorm:
        return VK_FORMAT_R16_UNORM;
    case RS::Format::R16Snorm:
        return VK_FORMAT_R16_SNORM;
    case RS::Format::R16Uint:
        return VK_FORMAT_R16_UINT;
    case RS::Format::R16Sint:
        return VK_FORMAT_R16_SINT;
    case RS::Format::R16Sfloat:
        return VK_FORMAT_R16_SFLOAT;
    case RS::Format::Rg16Unorm:
        return VK_FORMAT_R16G16_UNORM;
    case RS::Format::Rg16Snorm:
        return VK_FORMAT_R16G16_SNORM;
    case RS::Format::Rg16Uint:
        return VK_FORMAT_R16G16_UINT;
    case RS::Format::Rg16Sint:
        return VK_FORMAT_R16G16_SINT;
    case RS::Format::Rg16Sfloat:
        return VK_FORMAT_R16G16_SFLOAT;
    case RS::Format::Rgb16Unorm:
        return VK_FORMAT_R16G16B16_UNORM;
    case RS::Format::Rgb16Snorm:
        return VK_FORMAT_R16G16B16_SNORM;
    case RS::Format::Rgb16Uint:
        return VK_FORMAT_R16G16B16_UINT;
    case RS::Format::Rgb16Sint:
        return VK_FORMAT_R16G16B16_SINT;
    case RS::Format::Rgb16Sfloat:
        return VK_FORMAT_R16G16B16_SFLOAT;
    case RS::Format::Rgba16Unorm:
        return VK_FORMAT_R16G16B16A16_UNORM;
    case RS::Format::Rgba16Snorm:
        return VK_FORMAT_R16G16B16A16_SNORM;
    case RS::Format::Rgba16Uint:
        return VK_FORMAT_R16G16B16A16_UINT;
    case RS::Format::Rgba16Sint:
        return VK_FORMAT_R16G16B16A16_SINT;
    case RS::Format::Rgba16Sfloat:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case RS::Format::R32Uint:
        return VK_FORMAT_R32_UINT;
    case RS::Format::R32Sint:
        return VK_FORMAT_R32_SINT;
    case RS::Format::R32Sfloat:
        return VK_FORMAT_R32_SFLOAT;
    case RS::Format::Rg32Uint:
        return VK_FORMAT_R32G32_UINT;
    case RS::Format::Rg32Sint:
        return VK_FORMAT_R32G32_SINT;
    case RS::Format::Rg32Sfloat:
        return VK_FORMAT_R32G32_SFLOAT;
    case RS::Format::Rgb32Uint:
        return VK_FORMAT_R32G32B32_UINT;
    case RS::Format::Rgb32Sint:
        return VK_FORMAT_R32G32B32_SINT;
    case RS::Format::Rgb32Sfloat:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case RS::Format::Rgba32Uint:
        return VK_FORMAT_R32G32B32A32_UINT;
    case RS::Format::Rgba32Sint:
        return VK_FORMAT_R32G32B32A32_SINT;
    case RS::Format::Rgba32Sfloat:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case RS::Format::R64Uint:
        return VK_FORMAT_R64_UINT;
    case RS::Format::R64Sint:
        return VK_FORMAT_R64_SINT;
    case RS::Format::R64Sfloat:
        return VK_FORMAT_R64_SFLOAT;
    case RS::Format::Rg64Uint:
        return VK_FORMAT_R64G64_UINT;
    case RS::Format::Rg64Sint:
        return VK_FORMAT_R64G64_SINT;
    case RS::Format::Rg64Sfloat:
        return VK_FORMAT_R64G64_SFLOAT;
    case RS::Format::Rgb64Uint:
        return VK_FORMAT_R64G64B64_UINT;
    case RS::Format::Rgb64Sint:
        return VK_FORMAT_R64G64B64_SINT;
    case RS::Format::Rgb64Sfloat:
        return VK_FORMAT_R64G64B64_SFLOAT;
    case RS::Format::Rgba64Uint:
        return VK_FORMAT_R64G64B64A64_UINT;
    case RS::Format::Rgba64Sint:
        return VK_FORMAT_R64G64B64A64_SINT;
    case RS::Format::Rgba64Sfloat:
        return VK_FORMAT_R64G64B64A64_SFLOAT;
    case RS::Format::D16Unorm:
        return VK_FORMAT_D16_UNORM;
    case RS::Format::D32Sfloat:
        return VK_FORMAT_D32_SFLOAT;
    case RS::Format::S8Uint:
        return VK_FORMAT_S8_UINT;
    case RS::Format::D16UnormS8Uint:
        return VK_FORMAT_D16_UNORM_S8_UINT;
    case RS::Format::D24UnormS8Uint:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case RS::Format::D32SfloatS8Uint:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
    default:
        HE_UNREACHABLE();
    }
}

VkImageType VulkanRenderDriver::get_image_type(const RS::Dimension dimension)
{
    switch (dimension)
    {
    case RS::Dimension::Texture1D:
    case RS::Dimension::Texture1DArray:
        return VK_IMAGE_TYPE_1D;
    case RS::Dimension::Texture2D:
    case RS::Dimension::Texture2DArray:
        return VK_IMAGE_TYPE_2D;
    case RS::Dimension::Texture3D:
        return VK_IMAGE_TYPE_3D;
    case RS::Dimension::Unknown:
    default:
        HE_UNREACHABLE();
    }
}

VkImageUsageFlags VulkanRenderDriver::get_image_usage_flags(const BitFlags<RS::TextureUsage> texture_usage_flags, const RS::Format format)
{
    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (texture_usage_flags & RS::TextureUsage::Storage)
    {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }
    else
    {
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    if (texture_usage_flags & RS::TextureUsage::RenderAttachment)
    {
        switch (format)
        {
        case RS::Format::R8Unorm:
        case RS::Format::R8Snorm:
        case RS::Format::R8Uint:
        case RS::Format::R8Sint:
        case RS::Format::R8Srgb:
        case RS::Format::Rg8Unorm:
        case RS::Format::Rg8Snorm:
        case RS::Format::Rg8Uint:
        case RS::Format::Rg8Sint:
        case RS::Format::Rg8Srgb:
        case RS::Format::Rgb8Unorm:
        case RS::Format::Rgb8Snorm:
        case RS::Format::Rgb8Uint:
        case RS::Format::Rgb8Sint:
        case RS::Format::Rgb8Srgb:
        case RS::Format::Bgr8Unorm:
        case RS::Format::Bgr8Snorm:
        case RS::Format::Bgr8Uint:
        case RS::Format::Bgr8Sint:
        case RS::Format::Bgr8Srgb:
        case RS::Format::Rgba8Unorm:
        case RS::Format::Rgba8Snorm:
        case RS::Format::Rgba8Uint:
        case RS::Format::Rgba8Sint:
        case RS::Format::Rgba8Srgb:
        case RS::Format::Bgra8Unorm:
        case RS::Format::Bgra8Snorm:
        case RS::Format::Bgra8Uint:
        case RS::Format::Bgra8Sint:
        case RS::Format::Bgra8Srgb:
        case RS::Format::R16Unorm:
        case RS::Format::R16Snorm:
        case RS::Format::R16Uint:
        case RS::Format::R16Sint:
        case RS::Format::R16Sfloat:
        case RS::Format::Rg16Unorm:
        case RS::Format::Rg16Snorm:
        case RS::Format::Rg16Uint:
        case RS::Format::Rg16Sint:
        case RS::Format::Rg16Sfloat:
        case RS::Format::Rgb16Unorm:
        case RS::Format::Rgb16Snorm:
        case RS::Format::Rgb16Uint:
        case RS::Format::Rgb16Sint:
        case RS::Format::Rgb16Sfloat:
        case RS::Format::Rgba16Unorm:
        case RS::Format::Rgba16Snorm:
        case RS::Format::Rgba16Uint:
        case RS::Format::Rgba16Sint:
        case RS::Format::Rgba16Sfloat:
        case RS::Format::R32Uint:
        case RS::Format::R32Sint:
        case RS::Format::R32Sfloat:
        case RS::Format::Rg32Uint:
        case RS::Format::Rg32Sint:
        case RS::Format::Rg32Sfloat:
        case RS::Format::Rgb32Uint:
        case RS::Format::Rgb32Sint:
        case RS::Format::Rgb32Sfloat:
        case RS::Format::Rgba32Uint:
        case RS::Format::Rgba32Sint:
        case RS::Format::Rgba32Sfloat:
        case RS::Format::R64Uint:
        case RS::Format::R64Sint:
        case RS::Format::R64Sfloat:
        case RS::Format::Rg64Uint:
        case RS::Format::Rg64Sint:
        case RS::Format::Rg64Sfloat:
        case RS::Format::Rgb64Uint:
        case RS::Format::Rgb64Sint:
        case RS::Format::Rgb64Sfloat:
        case RS::Format::Rgba64Uint:
        case RS::Format::Rgba64Sint:
        case RS::Format::Rgba64Sfloat:
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            break;
        case RS::Format::D16Unorm:
        case RS::Format::D32Sfloat:
        case RS::Format::S8Uint:
        case RS::Format::D16UnormS8Uint:
        case RS::Format::D24UnormS8Uint:
        case RS::Format::D32SfloatS8Uint:
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            break;
        case RS::Format::Unknown:
        default:
            HE_UNREACHABLE();
        }
    }

    return usage;
}

VkImageAspectFlags VulkanRenderDriver::get_image_aspect_flags(const RS::Format format)
{
    switch (format)
    {
    case RS::Format::R8Unorm:
    case RS::Format::R8Snorm:
    case RS::Format::R8Uint:
    case RS::Format::R8Sint:
    case RS::Format::R8Srgb:
    case RS::Format::Rg8Unorm:
    case RS::Format::Rg8Snorm:
    case RS::Format::Rg8Uint:
    case RS::Format::Rg8Sint:
    case RS::Format::Rg8Srgb:
    case RS::Format::Rgb8Unorm:
    case RS::Format::Rgb8Snorm:
    case RS::Format::Rgb8Uint:
    case RS::Format::Rgb8Sint:
    case RS::Format::Rgb8Srgb:
    case RS::Format::Bgr8Unorm:
    case RS::Format::Bgr8Snorm:
    case RS::Format::Bgr8Uint:
    case RS::Format::Bgr8Sint:
    case RS::Format::Bgr8Srgb:
    case RS::Format::Rgba8Unorm:
    case RS::Format::Rgba8Snorm:
    case RS::Format::Rgba8Uint:
    case RS::Format::Rgba8Sint:
    case RS::Format::Rgba8Srgb:
    case RS::Format::Bgra8Unorm:
    case RS::Format::Bgra8Snorm:
    case RS::Format::Bgra8Uint:
    case RS::Format::Bgra8Sint:
    case RS::Format::Bgra8Srgb:
    case RS::Format::R16Unorm:
    case RS::Format::R16Snorm:
    case RS::Format::R16Uint:
    case RS::Format::R16Sint:
    case RS::Format::R16Sfloat:
    case RS::Format::Rg16Unorm:
    case RS::Format::Rg16Snorm:
    case RS::Format::Rg16Uint:
    case RS::Format::Rg16Sint:
    case RS::Format::Rg16Sfloat:
    case RS::Format::Rgb16Unorm:
    case RS::Format::Rgb16Snorm:
    case RS::Format::Rgb16Uint:
    case RS::Format::Rgb16Sint:
    case RS::Format::Rgb16Sfloat:
    case RS::Format::Rgba16Unorm:
    case RS::Format::Rgba16Snorm:
    case RS::Format::Rgba16Uint:
    case RS::Format::Rgba16Sint:
    case RS::Format::Rgba16Sfloat:
    case RS::Format::R32Uint:
    case RS::Format::R32Sint:
    case RS::Format::R32Sfloat:
    case RS::Format::Rg32Uint:
    case RS::Format::Rg32Sint:
    case RS::Format::Rg32Sfloat:
    case RS::Format::Rgb32Uint:
    case RS::Format::Rgb32Sint:
    case RS::Format::Rgb32Sfloat:
    case RS::Format::Rgba32Uint:
    case RS::Format::Rgba32Sint:
    case RS::Format::Rgba32Sfloat:
    case RS::Format::R64Uint:
    case RS::Format::R64Sint:
    case RS::Format::R64Sfloat:
    case RS::Format::Rg64Uint:
    case RS::Format::Rg64Sint:
    case RS::Format::Rg64Sfloat:
    case RS::Format::Rgb64Uint:
    case RS::Format::Rgb64Sint:
    case RS::Format::Rgb64Sfloat:
    case RS::Format::Rgba64Uint:
    case RS::Format::Rgba64Sint:
    case RS::Format::Rgba64Sfloat:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    case RS::Format::D16Unorm:
    case RS::Format::D32Sfloat:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case RS::Format::S8Uint:
        return VK_IMAGE_ASPECT_STENCIL_BIT;
    case RS::Format::D16UnormS8Uint:
    case RS::Format::D24UnormS8Uint:
    case RS::Format::D32SfloatS8Uint:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    case RS::Format::Unknown:
    default:
        HE_UNREACHABLE();
    }
}

VkImageViewType VulkanRenderDriver::get_image_view_type(const RS::Dimension dimension)
{
    switch (dimension)
    {
    case RS::Dimension::Texture1D:
        return VK_IMAGE_VIEW_TYPE_1D;
    case RS::Dimension::Texture1DArray:
        return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    case RS::Dimension::Texture2D:
        return VK_IMAGE_VIEW_TYPE_2D;
    case RS::Dimension::Texture2DArray:
        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case RS::Dimension::Texture3D:
        return VK_IMAGE_VIEW_TYPE_3D;
    case RS::Dimension::Unknown:
    default:
        HE_UNREACHABLE();
    }
}

VkPrimitiveTopology VulkanRenderDriver::get_primitive_topology(const RS::PrimitiveTopology primitive_topology)
{
    switch (primitive_topology)
    {
    case RS::PrimitiveTopology::PointList:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case RS::PrimitiveTopology::LineList:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case RS::PrimitiveTopology::LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case RS::PrimitiveTopology::TriangleList:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case RS::PrimitiveTopology::TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case RS::PrimitiveTopology::TriangleFan:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    default:
        HE_UNREACHABLE();
    }
}

VkPolygonMode VulkanRenderDriver::get_polygon_mode(const RS::PolygonMode polygon_mode)
{
    switch (polygon_mode)
    {
    case RS::PolygonMode::Fill:
        return VK_POLYGON_MODE_FILL;
    case RS::PolygonMode::Line:
        return VK_POLYGON_MODE_LINE;
    case RS::PolygonMode::Point:
        return VK_POLYGON_MODE_POINT;
    default:
        HE_UNREACHABLE();
    }
}

VkCullModeFlags VulkanRenderDriver::get_cull_mode_flags(const RS::Face face)
{
    switch (face)
    {
    case RS::Face::None:
        return VK_CULL_MODE_NONE;
    case RS::Face::Front:
        return VK_CULL_MODE_FRONT_BIT;
    case RS::Face::Back:
        return VK_CULL_MODE_BACK_BIT;
    default:
        HE_UNREACHABLE();
    }
}

VkFrontFace VulkanRenderDriver::get_front_face(const RS::FrontFace front_face)
{
    switch (front_face)
    {
    case RS::FrontFace::CounterClockwise:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    case RS::FrontFace::Clockwise:
        return VK_FRONT_FACE_CLOCKWISE;
    default:
        HE_UNREACHABLE();
    }
}

VkCompareOp VulkanRenderDriver::get_compare_operation(const RS::CompareOperation compare_operation)
{
    switch (compare_operation)
    {
    case RS::CompareOperation::Never:
        return VK_COMPARE_OP_NEVER;
    case RS::CompareOperation::Less:
        return VK_COMPARE_OP_LESS;
    case RS::CompareOperation::Equal:
        return VK_COMPARE_OP_EQUAL;
    case RS::CompareOperation::LessEqual:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case RS::CompareOperation::Greater:
        return VK_COMPARE_OP_GREATER;
    case RS::CompareOperation::NotEqual:
        return VK_COMPARE_OP_NOT_EQUAL;
    case RS::CompareOperation::GreaterEqual:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case RS::CompareOperation::Always:
        return VK_COMPARE_OP_ALWAYS;
    default:
        HE_UNREACHABLE();
    }
}

VkBlendFactor VulkanRenderDriver::get_blend_factor(const RS::BlendFactor blend_factor)
{
    switch (blend_factor)
    {
    case RS::BlendFactor::Zero:
        return VK_BLEND_FACTOR_ZERO;
    case RS::BlendFactor::One:
        return VK_BLEND_FACTOR_ONE;
    case RS::BlendFactor::SrcColor:
        return VK_BLEND_FACTOR_SRC_COLOR;
    case RS::BlendFactor::OneMinusSrcColor:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case RS::BlendFactor::DstColor:
        return VK_BLEND_FACTOR_DST_COLOR;
    case RS::BlendFactor::OneMinusDstColor:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case RS::BlendFactor::SrcAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case RS::BlendFactor::OneMinusSrcAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case RS::BlendFactor::DstAlpha:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case RS::BlendFactor::OneMinusDstAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case RS::BlendFactor::ConstantColor:
        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case RS::BlendFactor::OneMinusConstantColor:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case RS::BlendFactor::ConstantAlpha:
        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case RS::BlendFactor::OneMinusConstantAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    case RS::BlendFactor::SrcAlphaSaturate:
        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case RS::BlendFactor::Src1Color:
        return VK_BLEND_FACTOR_SRC1_COLOR;
    case RS::BlendFactor::OneMinusSrc1Color:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case RS::BlendFactor::Src1Alpha:
        return VK_BLEND_FACTOR_SRC1_ALPHA;
    case RS::BlendFactor::OneMinusSrc1Alpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    default:
        HE_UNREACHABLE();
    }
}

VkBlendOp VulkanRenderDriver::get_blend_operation(const RS::BlendOperation blend_operation)
{
    switch (blend_operation)
    {
    case RS::BlendOperation::Add:
        return VK_BLEND_OP_ADD;
    case RS::BlendOperation::Subtract:
        return VK_BLEND_OP_SUBTRACT;
    case RS::BlendOperation::ReverseSubtract:
        return VK_BLEND_OP_REVERSE_SUBTRACT;
    case RS::BlendOperation::Min:
        return VK_BLEND_OP_MIN;
    case RS::BlendOperation::Max:
        return VK_BLEND_OP_MAX;
    default:
        HE_UNREACHABLE();
    }
}

VkColorComponentFlags VulkanRenderDriver::get_color_component_flags(const BitFlags<RS::ColorWrites> color_writes)
{
    VkColorComponentFlags color_component_flags = 0;
    if (color_writes & RS::ColorWrites::R)
    {
        color_component_flags |= VK_COLOR_COMPONENT_R_BIT;
    }

    if (color_writes & RS::ColorWrites::G)
    {
        color_component_flags |= VK_COLOR_COMPONENT_G_BIT;
    }

    if (color_writes & RS::ColorWrites::B)
    {
        color_component_flags |= VK_COLOR_COMPONENT_B_BIT;
    }

    if (color_writes & RS::ColorWrites::A)
    {
        color_component_flags |= VK_COLOR_COMPONENT_A_BIT;
    }

    return color_component_flags;
}