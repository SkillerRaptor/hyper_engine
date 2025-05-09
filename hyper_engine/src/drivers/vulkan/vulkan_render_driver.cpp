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
#include <tracy/Tracy.hpp>
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

void VulkanRenderDriver::initialize(WindowSystem &window_system, const WindowId window)
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

    const glm::uvec2 size = window_system.get_window_size(window);
    create_swapchain(size.x, size.y);

    window_system.register_listener<WindowResizeEvent>(HE_BIND_FUNCTION(on_resize));

    find_descriptor_counts();
    create_descriptor_pool();
    create_descriptor_set_layouts();
    create_descriptor_sets();

    HE_INFO("Successfully initialized VulkanRenderDriver");
}

void VulkanRenderDriver::shutdown()
{
    for (const VkDescriptorSetLayout &descriptor_set_layout : m_descriptor_set_layouts)
    {
        vkDestroyDescriptorSetLayout(m_device, descriptor_set_layout, nullptr);
    }

    vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);

    destroy_swapchain();
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);

    vmaDestroyAllocator(m_allocator);

    vkDestroyDevice(m_device, nullptr);

    if (m_validation_layer_enabled)
    {
        vkDestroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
    }

    vkDestroyInstance(m_instance, nullptr);
}

void VulkanRenderDriver::wait_idle() const
{
    const VkResult result = vkDeviceWaitIdle(m_device);
    HE_VK_CHECK(result, vkDeviceWaitIdle);
}

std::vector<Texture *> VulkanRenderDriver::query_swapchain_textures()
{
    return m_swapchain_textures;
}

Buffer *VulkanRenderDriver::create_buffer(
    const std::optional<std::string> &label,
    const uint64_t byte_size,
    const BitFlags<BufferUsage> usage,
    const bool staging) const
{
    const VkBufferCreateInfo buffer_create_info = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = byte_size,
        .usage = get_buffer_usage_flags(usage),
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

void VulkanRenderDriver::destroy_buffer(const Buffer *buffer) const
{
    const VulkanBuffer *vulkan_buffer = reinterpret_cast<const VulkanBuffer *>(buffer);

    vmaDestroyBuffer(m_allocator, vulkan_buffer->buffer, vulkan_buffer->allocation);
    delete vulkan_buffer;
}

void *VulkanRenderDriver::map_buffer(const Buffer *buffer) const
{
    const VulkanBuffer *vulkan_buffer = reinterpret_cast<const VulkanBuffer *>(buffer);

    void *mapped_ptr = nullptr;
    vmaMapMemory(m_allocator, vulkan_buffer->allocation, &mapped_ptr);

    HE_ASSERT(mapped_ptr != nullptr);

    return mapped_ptr;
}

void VulkanRenderDriver::unmap_buffer(const Buffer *buffer) const
{
    const VulkanBuffer *vulkan_buffer = reinterpret_cast<const VulkanBuffer *>(buffer);

    vmaUnmapMemory(m_allocator, vulkan_buffer->allocation);
}

Shader *VulkanRenderDriver::create_shader(
    const std::optional<std::string> &label,
    const ShaderType type,
    const std::string_view entry,
    const std::string_view path) const
{
    const std::vector<uint8_t> data = filesystem::read_file(path);
    HE_ASSERT(!data.empty());

    const ShaderCompilationDescriptor shader_compilation_descriptor = {
        .entry_name = entry.data(),
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
            case ShaderType::Compute:
                return "compute";
            case ShaderType::Fragment:
                return "fragment";
            case ShaderType::Vertex:
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

    VkShaderModule shader_module = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateShaderModule(m_device, &shader_module_create_info, nullptr, &shader_module), vkCreateShaderModule);
    HE_ASSERT(shader_module != VK_NULL_HANDLE);

    return new VulkanShader({
        .shader_module = shader_module,
    });
}

void VulkanRenderDriver::destroy_shader(const Shader *shader) const
{
    const VulkanShader *vulkan_shader = reinterpret_cast<const VulkanShader *>(shader);

    vkDestroyShaderModule(m_device, vulkan_shader->shader_module, nullptr);
    delete vulkan_shader;
}

Sampler *VulkanRenderDriver::create_sampler(
    const std::optional<std::string> &label,
    const Filter mag_filter,
    const Filter min_filter,
    const Filter mipmap_filter,
    const AddressMode address_mode_u,
    const AddressMode address_mode_v,
    const AddressMode address_mode_w,
    const float mip_lod_bias,
    const CompareOperation compare_operation,
    const float min_lod,
    const float max_lod,
    const BorderColor border_color) const
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

    VkSampler sampler = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateSampler(m_device, &sampler_create_info, nullptr, &sampler), vkCreateSampler);
    HE_ASSERT(sampler != VK_NULL_HANDLE);

    return new VulkanSampler({
        .sampler = sampler,
    });
}

void VulkanRenderDriver::destroy_sampler(const Sampler *sampler) const
{
    const VulkanSampler *vulkan_sampler = reinterpret_cast<const VulkanSampler *>(sampler);

    vkDestroySampler(m_device, vulkan_sampler->sampler, nullptr);
    delete vulkan_sampler;
}

Texture *VulkanRenderDriver::create_texture(
    const std::optional<std::string> &label,
    const uint32_t width,
    const uint32_t height,
    const uint32_t depth,
    const uint32_t array_size,
    const uint32_t mip_levels,
    const Format format,
    const Dimension dimension,
    const BitFlags<TextureUsage> usage) const
{
    const VkImageCreateInfo image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = get_image_type(dimension),
        .format = get_format(format),
        .extent =
            {
                .width = width,
                .height = height,
                .depth = depth,
            },
        .mipLevels = mip_levels,
        .arrayLayers = array_size,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = get_image_usage_flags(usage, format),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    constexpr VmaAllocationCreateInfo allocation_create_info = {
        .flags = 0,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 0,
    };

    VkImage image = VK_NULL_HANDLE;
    VmaAllocation allocation = VK_NULL_HANDLE;
    HE_VK_CHECK(vmaCreateImage(m_allocator, &image_create_info, &allocation_create_info, &image, &allocation, nullptr), vmaCreateImage);
    HE_ASSERT(image != VK_NULL_HANDLE);
    HE_ASSERT(allocation != VK_NULL_HANDLE);

    const VkImageView image_view = create_internal_image_view(image, dimension, format);

    return new VulkanTexture({
        .image = image,
        .allocation = allocation,
        .image_view = image_view,
        .image_layout = VK_IMAGE_LAYOUT_UNDEFINED,
    });
}

void VulkanRenderDriver::destroy_texture(const Texture *texture) const
{
    const VulkanTexture *vulkan_texture = reinterpret_cast<const VulkanTexture *>(texture);

    vkDestroyImageView(m_device, vulkan_texture->image_view, nullptr);
    if (vulkan_texture->allocation != VK_NULL_HANDLE)
    {
        vmaDestroyImage(m_allocator, vulkan_texture->image, vulkan_texture->allocation);
    }
    delete vulkan_texture;
}

PipelineLayout *VulkanRenderDriver::create_pipeline_layout(const std::optional<std::string> &label, const uint32_t push_constant_size) const
{
    const VkPushConstantRange push_constant_range = {
        .stageFlags = VK_SHADER_STAGE_ALL,
        .offset = 0,
        .size = push_constant_size,
    };

    const VkPipelineLayoutCreateInfo pipeline_layout_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = static_cast<uint32_t>(m_descriptor_set_layouts.size()),
        .pSetLayouts = m_descriptor_set_layouts.data(),
        .pushConstantRangeCount = static_cast<uint32_t>(push_constant_size == 0 ? 0 : 1),
        .pPushConstantRanges = push_constant_size == 0 ? nullptr : &push_constant_range,
    };

    VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreatePipelineLayout(m_device, &pipeline_layout_create_info, nullptr, &pipeline_layout), vkCreatePipelineLayout);
    HE_ASSERT(pipeline_layout != VK_NULL_HANDLE);

    return new VulkanPipelineLayout({
        .pipeline_layout = pipeline_layout,
    });
}

void VulkanRenderDriver::destroy_pipeline_layout(const PipelineLayout *pipeline_layout) const
{
    const VulkanPipelineLayout *vulkan_pipeline_layout = reinterpret_cast<const VulkanPipelineLayout *>(pipeline_layout);

    vkDestroyPipelineLayout(m_device, vulkan_pipeline_layout->pipeline_layout, nullptr);
    delete vulkan_pipeline_layout;
}

ComputePipeline *VulkanRenderDriver::create_compute_pipeline(
    const std::optional<std::string> &label,
    const PipelineLayout *layout,
    const Shader *shader) const
{
    const VulkanPipelineLayout *vulkan_layout = reinterpret_cast<const VulkanPipelineLayout *>(layout);
    const VulkanShader *vulkan_shader = reinterpret_cast<const VulkanShader *>(shader);

    const VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = vulkan_shader->shader_module,
        .pName = vulkan_shader->entry.c_str(),
        .pSpecializationInfo = nullptr,
    };

    const VkComputePipelineCreateInfo compute_pipeline_create_info = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = pipeline_shader_stage_create_info,
        .layout = vulkan_layout->pipeline_layout,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipeline pipeline = VK_NULL_HANDLE;
    HE_VK_CHECK(
        vkCreateComputePipelines(m_device, VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &pipeline), vkCreateComputePipelines);
    HE_ASSERT(pipeline != VK_NULL_HANDLE);

    return new VulkanComputePipeline({
        .pipeline = pipeline,
    });
}

void VulkanRenderDriver::destroy_compute_pipeline(const ComputePipeline *compute_pipeline) const
{
    const VulkanComputePipeline *vulkan_compute_pipeline = reinterpret_cast<const VulkanComputePipeline *>(compute_pipeline);

    vkDestroyPipeline(m_device, vulkan_compute_pipeline->pipeline, nullptr);
    delete vulkan_compute_pipeline;
}

RenderPipeline *VulkanRenderDriver::create_render_pipeline(
    const std::optional<std::string> &label,
    const PipelineLayout *layout,
    const Shader *vertex_shader,
    const Shader *fragment_shader,
    const std::vector<ColorAttachmentState> &color_attachment_states,
    const PrimitiveState &primitive_state,
    const DepthStencilState &depth_stencil_state) const
{
    const VulkanShader *vulkan_vertex_shader = reinterpret_cast<const VulkanShader *>(vertex_shader);

    const VkPipelineShaderStageCreateInfo vertex_pipeline_shader_stage_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vulkan_vertex_shader->shader_module,
        .pName = vulkan_vertex_shader->entry.c_str(),
        .pSpecializationInfo = nullptr,
    };

    const VulkanShader *vulkan_fragment_shader = reinterpret_cast<const VulkanShader *>(fragment_shader);

    const VkPipelineShaderStageCreateInfo fragment_pipeline_shader_stage_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = vulkan_fragment_shader->shader_module,
        .pName = vulkan_fragment_shader->entry.c_str(),
        .pSpecializationInfo = nullptr,
    };

    const std::array<VkPipelineShaderStageCreateInfo, 2> shader_stage_create_infos = {
        vertex_pipeline_shader_stage_create_info,
        fragment_pipeline_shader_stage_create_info,
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

    const VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = get_primitive_topology(primitive_state.topology),
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

    // FIXME: Add depth bounds & stencil
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
    for (const ColorAttachmentState &color_attachment_state : color_attachment_states)
    {
        const BlendState &blend_state = color_attachment_state.blend_state;

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
    for (const ColorAttachmentState &color_attachment_state : color_attachment_states)
    {
        color_attachment_formats.push_back(get_format(color_attachment_state.format));
    }

    const VkPipelineRenderingCreateInfo rendering_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = static_cast<uint32_t>(color_attachment_formats.size()),
        .pColorAttachmentFormats = color_attachment_formats.data(),
        .depthAttachmentFormat = get_format(depth_stencil_state.depth_format),
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    const VulkanPipelineLayout *vulkan_pipeline_layout = reinterpret_cast<const VulkanPipelineLayout *>(layout);

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

    VkPipeline pipeline = VK_NULL_HANDLE;
    HE_VK_CHECK(
        vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &pipeline), vkCreateGraphicsPipelines);
    HE_ASSERT(pipeline != VK_NULL_HANDLE);

    return new VulkanRenderPipeline({
        .pipeline = pipeline,
    });
}

void VulkanRenderDriver::destroy_render_pipeline(const RenderPipeline *render_pipeline) const
{
    const VulkanRenderPipeline *vulkan_render_pipeline = reinterpret_cast<const VulkanRenderPipeline *>(render_pipeline);

    vkDestroyPipeline(m_device, vulkan_render_pipeline->pipeline, nullptr);
    delete vulkan_render_pipeline;
}

CommandBuffer *VulkanRenderDriver::create_command_buffer() const
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

void VulkanRenderDriver::destroy_command_buffer(const CommandBuffer *command_buffer) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);

    vkDestroyCommandPool(m_device, vulkan_command_buffer->command_pool, nullptr);
    vkDestroyFence(m_device, vulkan_command_buffer->render_fence, nullptr);
    vkDestroySemaphore(m_device, vulkan_command_buffer->submit_semaphore, nullptr);
    delete vulkan_command_buffer;
}

void VulkanRenderDriver::acquire_command_buffer(const CommandBuffer *command_buffer) const
{
    ZoneScopedNC("AcquireCommandBuffer", 0xff8a02);

    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);

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

void VulkanRenderDriver::submit_command_buffer(CommandBuffer *command_buffer) const
{
    ZoneScopedNC("SubmitCommandBuffer", 0xff8a02);

    VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<VulkanCommandBuffer *>(command_buffer);

    if (vulkan_command_buffer->swapchain_texture_acquired)
    {
        Texture *swapchain_texture = m_swapchain_textures[m_swapchain_texture_index];
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

void VulkanRenderDriver::bind_buffer(const Buffer *buffer) const
{
    HE_ASSERT(buffer->handle.has_value());

    const VulkanBuffer *vulkan_buffer = reinterpret_cast<const VulkanBuffer *>(buffer);

    const VkDescriptorBufferInfo buffer_info = {
        .buffer = vulkan_buffer->buffer,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    const VkWriteDescriptorSet descriptor_write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptor_sets[0],
        .dstBinding = 0,
        .dstArrayElement = vulkan_buffer->handle.value().handle(),
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &buffer_info,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device, 1, &descriptor_write, 0, nullptr);
}

void VulkanRenderDriver::bind_sampler(const Sampler *sampler) const
{
    HE_ASSERT(sampler->handle.has_value());

    const VulkanSampler *vulkan_sampler = reinterpret_cast<const VulkanSampler *>(sampler);

    const VkDescriptorImageInfo image_info = {
        .sampler = vulkan_sampler->sampler,
        .imageView = VK_NULL_HANDLE,
        .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const VkWriteDescriptorSet descriptor_write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptor_sets[3],
        .dstBinding = 0,
        .dstArrayElement = vulkan_sampler->handle.value().handle(),
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
        .pImageInfo = &image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device, 1, &descriptor_write, 0, nullptr);
}

void VulkanRenderDriver::bind_texture(const Texture *texture) const
{
    HE_ASSERT(texture->handle.has_value());

    const VulkanTexture *vulkan_texture = reinterpret_cast<const VulkanTexture *>(texture);

    const VkDescriptorImageInfo image_info = {
        .sampler = VK_NULL_HANDLE,
        .imageView = vulkan_texture->image_view,
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    };

    const VkWriteDescriptorSet descriptor_write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptor_sets[(vulkan_texture->usage & TextureUsage::Storage) ? 2 : 1],
        .dstBinding = 0,
        .dstArrayElement = vulkan_texture->handle.value().handle(),
        .descriptorCount = 1,
        .descriptorType = (vulkan_texture->usage & TextureUsage::Storage) ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo = &image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device, 1, &descriptor_write, 0, nullptr);
}

void VulkanRenderDriver::copy_buffer_to_buffer(
    const CommandBuffer *command_buffer,
    const Buffer *src,
    const uint32_t src_offset,
    const Buffer *dst,
    const uint32_t dst_offset,
    const uint32_t size) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanBuffer *vulkan_src = reinterpret_cast<const VulkanBuffer *>(src);
    const VulkanBuffer *vulkan_dst = reinterpret_cast<const VulkanBuffer *>(dst);

    const VkBufferCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
        .pNext = nullptr,
        .srcOffset = src_offset,
        .dstOffset = dst_offset,
        .size = size,
    };

    const VkCopyBufferInfo2 copy_buffer_info = {
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .pNext = nullptr,
        .srcBuffer = vulkan_src->buffer,
        .dstBuffer = vulkan_dst->buffer,
        .regionCount = 1,
        .pRegions = &region,
    };

    vkCmdCopyBuffer2(vulkan_command_buffer->command_buffer, &copy_buffer_info);
}

void VulkanRenderDriver::copy_buffer_to_texture(
    const CommandBuffer *command_buffer,
    const Buffer *src,
    const uint32_t src_offset,
    Texture *dst,
    const Offset3d dst_offset,
    const Extent3d dst_extent,
    const uint32_t dst_mip_level,
    const uint32_t dst_array_index) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanBuffer *vulkan_src = reinterpret_cast<const VulkanBuffer *>(src);
    const VulkanTexture *vulkan_dst = reinterpret_cast<const VulkanTexture *>(dst);

    transition_texture_layout(command_buffer, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    const VkImageSubresourceLayers subresource_layers = {
        .aspectMask = get_image_aspect_flags(vulkan_dst->format),
        .mipLevel = dst_mip_level,
        .baseArrayLayer = dst_array_index,
        .layerCount = 1,
    };

    const VkBufferImageCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
        .pNext = nullptr,
        .bufferOffset = src_offset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = subresource_layers,
        .imageOffset =
            {
                .x = dst_offset.x,
                .y = dst_offset.y,
                .z = dst_offset.z,
            },
        .imageExtent =
            {
                .width = dst_extent.width,
                .height = dst_extent.height,
                .depth = dst_extent.depth,
            },
    };

    const VkCopyBufferToImageInfo2 copy_buffer_to_image_info = {
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
        .pNext = nullptr,
        .srcBuffer = vulkan_src->buffer,
        .dstImage = vulkan_dst->image,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount = 1,
        .pRegions = &region,
    };

    vkCmdCopyBufferToImage2(vulkan_command_buffer->command_buffer, &copy_buffer_to_image_info);
}

void VulkanRenderDriver::copy_texture_to_buffer(
    const CommandBuffer *command_buffer,
    Texture *src,
    const Offset3d src_offset,
    const Extent3d src_extent,
    const uint32_t src_mip_level,
    const uint32_t src_array_index,
    const Buffer *dst,
    const uint32_t dst_offset) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanTexture *vulkan_src = reinterpret_cast<const VulkanTexture *>(src);
    const VulkanBuffer *vulkan_dst = reinterpret_cast<const VulkanBuffer *>(dst);

    transition_texture_layout(command_buffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    const VkImageSubresourceLayers subresource_layers = {
        .aspectMask = get_image_aspect_flags(vulkan_src->format),
        .mipLevel = src_mip_level,
        .baseArrayLayer = src_array_index,
        .layerCount = 1,
    };

    const VkBufferImageCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
        .pNext = nullptr,
        .bufferOffset = dst_offset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = subresource_layers,
        .imageOffset =
            {
                .x = src_offset.x,
                .y = src_offset.y,
                .z = src_offset.z,
            },
        .imageExtent =
            {
                .width = src_extent.width,
                .height = src_extent.height,
                .depth = src_extent.depth,
            },
    };

    const VkCopyImageToBufferInfo2 copy_image_to_buffer_info = {
        .sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2,
        .pNext = nullptr,
        .srcImage = vulkan_src->image,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstBuffer = vulkan_dst->buffer,
        .regionCount = 1,
        .pRegions = &region,
    };

    vkCmdCopyImageToBuffer2(vulkan_command_buffer->command_buffer, &copy_image_to_buffer_info);
}

void VulkanRenderDriver::copy_texture_to_texture(
    const CommandBuffer *command_buffer,
    Texture *src,
    const Offset3d src_offset,
    const uint32_t src_mip_level,
    const uint32_t src_array_index,
    Texture *dst,
    const Offset3d dst_offset,
    const uint32_t dst_mip_level,
    const uint32_t dst_array_index,
    const Extent3d extent) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanTexture *vulkan_src = reinterpret_cast<const VulkanTexture *>(src);
    const VulkanTexture *vulkan_dst = reinterpret_cast<const VulkanTexture *>(dst);

    transition_texture_layout(command_buffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    transition_texture_layout(command_buffer, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    const VkImageCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2,
        .pNext = nullptr,
        .srcSubresource =
            {
                .aspectMask = get_image_aspect_flags(vulkan_src->format),
                .mipLevel = src_mip_level,
                .baseArrayLayer = src_array_index,
                .layerCount = 1,
            },
        .srcOffset =
            {
                .x = src_offset.x,
                .y = src_offset.y,
                .z = src_offset.z,
            },
        .dstSubresource =
            {
                .aspectMask = get_image_aspect_flags(vulkan_dst->format),
                .mipLevel = dst_mip_level,
                .baseArrayLayer = dst_array_index,
                .layerCount = 1,
            },
        .dstOffset =
            {
                .x = dst_offset.x,
                .y = dst_offset.y,
                .z = dst_offset.z,
            },
        .extent =
            {
                .width = extent.width,
                .height = extent.height,
                .depth = extent.depth,
            },
    };

    const VkCopyImageInfo2 copy_image_info = {
        .sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2,
        .pNext = nullptr,
        .srcImage = vulkan_src->image,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage = vulkan_dst->image,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount = 1,
        .pRegions = &region,
    };

    vkCmdCopyImage2(vulkan_command_buffer->command_buffer, &copy_image_info);
}

void VulkanRenderDriver::push_constants(
    const CommandBuffer *command_buffer,
    const PipelineLayout *pipeline_layout,
    const void *data,
    const uint32_t size)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanPipelineLayout *vulkan_pipeline_layout = reinterpret_cast<const VulkanPipelineLayout *>(pipeline_layout);

    vkCmdPushConstants(vulkan_command_buffer->command_buffer, vulkan_pipeline_layout->pipeline_layout, VK_SHADER_STAGE_ALL, 0, size, data);
}

std::pair<uint32_t, bool> VulkanRenderDriver::acquire_swapchain_texture(const CommandBuffer *command_buffer)
{
    ZoneScopedNC("AcquireSwapchainTexture", 0xff8a02);

    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);

    bool recreated = false;
    if (m_swapchain_out_of_date)
    {
        recreate_swapchain();
        recreated = true;
    }

    while (true)
    {
        const VkResult result = vkAcquireNextImageKHR(
            m_device,
            m_swapchain,
            std::numeric_limits<uint64_t>::max(),
            VK_NULL_HANDLE,
            vulkan_command_buffer->render_fence,
            &m_swapchain_texture_index);

        if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
        {
            break;
        }

        recreate_swapchain();
        recreated = true;
    }

    return std::make_pair(m_swapchain_texture_index, recreated);
}

void VulkanRenderDriver::present()
{
    ZoneScopedNC("Present", 0xff8a02);

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

    const VkResult result = vkQueuePresentKHR(m_queue, &present_info);
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_swapchain_out_of_date = true;
    }
    else
    {
        HE_VK_CHECK(result, vkQueuePresentKHR);
    }
}

void VulkanRenderDriver::begin_gpu_marker(const CommandBuffer *command_buffer, const Label label) const
{
    if (!m_validation_layer_enabled)
    {
        return;
    }

    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);

    const float r = static_cast<float>(label.color.r) / 255.0f;
    const float g = static_cast<float>(label.color.g) / 255.0f;
    const float b = static_cast<float>(label.color.b) / 255.0f;
    const VkDebugUtilsLabelEXT debug_label = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        .pNext = nullptr,
        .pLabelName = label.name.data(),
        .color =
            {
                r,
                g,
                b,
                1.0f,
            },
    };

    vkCmdBeginDebugUtilsLabelEXT(vulkan_command_buffer->command_buffer, &debug_label);
}

void VulkanRenderDriver::end_gpu_marker(const CommandBuffer *command_buffer) const
{
    if (!m_validation_layer_enabled)
    {
        return;
    }

    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);

    vkCmdEndDebugUtilsLabelEXT(vulkan_command_buffer->command_buffer);
}

void VulkanRenderDriver::begin_compute_pass(const CommandBuffer *command_buffer) const
{
    (void) command_buffer;

    // NOTE: noop
}

void VulkanRenderDriver::end_compute_pass(const CommandBuffer *command_buffer) const
{
    (void) command_buffer;

    // NOTE: noop
}

void VulkanRenderDriver::bind_compute_pipeline(const CommandBuffer *command_buffer, const ComputePipeline *pipeline) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanComputePipeline *vulkan_compute_pipeline = reinterpret_cast<const VulkanComputePipeline *>(pipeline);
    const VulkanPipelineLayout *vulkan_pipeline_layout = reinterpret_cast<const VulkanPipelineLayout *>(vulkan_compute_pipeline->layout);

    vkCmdBindDescriptorSets(
        vulkan_command_buffer->command_buffer,
        VK_PIPELINE_BIND_POINT_COMPUTE,
        vulkan_pipeline_layout->pipeline_layout,
        0,
        static_cast<uint32_t>(m_descriptor_sets.size()),
        m_descriptor_sets.data(),
        0,
        nullptr);

    vkCmdBindPipeline(vulkan_command_buffer->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_compute_pipeline->pipeline);
}

void VulkanRenderDriver::begin_render_pass(
    const CommandBuffer *command_buffer,
    const std::vector<RenderPassColorAttachment> &color_attachments,
    const std::optional<RenderPassDepthStencilAttachment> &depth_stencil_attachment) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);

    // FIXME: This always uses the first color attachment
    const VkExtent2D extent = {
        .width = color_attachments[0].texture->width,
        .height = color_attachments[0].texture->height,
    };

    const VkRect2D render_area = {
        .offset =
            {
                .x = 0,
                .y = 0,
            },
        .extent = extent,
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

    std::vector<VkRenderingAttachmentInfo> rendering_color_attachments = {};
    for (const RenderPassColorAttachment &color_attachment : color_attachments)
    {
        const VulkanTexture *vulkan_texture = reinterpret_cast<const VulkanTexture *>(color_attachment.texture);

        transition_texture_layout(command_buffer, color_attachment.texture, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        const VkRenderingAttachmentInfo color_attachment_info = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = vulkan_texture->image_view,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = get_attachment_load_operation(color_attachment.operations.load_operation),
            .storeOp = get_attachment_store_operation(color_attachment.operations.store_operation),
            .clearValue = clear_value,
        };

        rendering_color_attachments.push_back(color_attachment_info);
    }

    std::optional<VkRenderingAttachmentInfo> depth_attachment_info = std::nullopt;
    if (depth_stencil_attachment.has_value())
    {
        const RenderPassDepthStencilAttachment &render_pass_depth_stencil_attachment = depth_stencil_attachment.value();
        const VulkanTexture *vulkan_texture = reinterpret_cast<const VulkanTexture *>(render_pass_depth_stencil_attachment.texture);

        transition_texture_layout(
            command_buffer, render_pass_depth_stencil_attachment.texture, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        constexpr VkClearValue depth_clear_value = {
            .depthStencil =
                {
                    .depth = 1.0f,
                    .stencil = 0,
                },
        };

        depth_attachment_info = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = vulkan_texture->image_view,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = get_attachment_load_operation(render_pass_depth_stencil_attachment.depth_operations.load_operation),
            .storeOp = get_attachment_store_operation(render_pass_depth_stencil_attachment.depth_operations.store_operation),
            .clearValue = depth_clear_value,
        };
    }

    const VkRenderingInfo rendering_info = {
        .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderArea = render_area,
        .layerCount = 1,
        .viewMask = 0,
        .colorAttachmentCount = static_cast<uint32_t>(rendering_color_attachments.size()),
        .pColorAttachments = rendering_color_attachments.data(),
        .pDepthAttachment = depth_attachment_info.has_value() ? &depth_attachment_info.value() : nullptr,
        .pStencilAttachment = nullptr,
    };

    const VkViewport viewport = {
        .x = 0.0,
        .y = 0,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0,
        .maxDepth = 1.0,
    };

    vkCmdBeginRendering(vulkan_command_buffer->command_buffer, &rendering_info);
    vkCmdSetViewport(vulkan_command_buffer->command_buffer, 0, 1, &viewport);
    vkCmdSetScissor(vulkan_command_buffer->command_buffer, 0, 1, &render_area);
}

void VulkanRenderDriver::end_render_pass(const CommandBuffer *command_buffer) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);

    vkCmdEndRendering(vulkan_command_buffer->command_buffer);
}

void VulkanRenderDriver::bind_render_pipeline(const CommandBuffer *command_buffer, const RenderPipeline *pipeline) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanRenderPipeline *vulkan_render_pipeline = reinterpret_cast<const VulkanRenderPipeline *>(pipeline);
    const VulkanPipelineLayout *vulkan_pipeline_layout = reinterpret_cast<const VulkanPipelineLayout *>(vulkan_render_pipeline->layout);

    vkCmdBindDescriptorSets(
        vulkan_command_buffer->command_buffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        vulkan_pipeline_layout->pipeline_layout,
        0,
        static_cast<uint32_t>(m_descriptor_sets.size()),
        m_descriptor_sets.data(),
        0,
        nullptr);

    vkCmdBindPipeline(vulkan_command_buffer->command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, vulkan_render_pipeline->pipeline);
}

void VulkanRenderDriver::bind_index_buffer(const CommandBuffer *command_buffer, const Buffer *buffer) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanBuffer *vulkan_buffer = reinterpret_cast<const VulkanBuffer *>(buffer);

    vkCmdBindIndexBuffer(vulkan_command_buffer->command_buffer, vulkan_buffer->buffer, 0, VK_INDEX_TYPE_UINT32);
}

void VulkanRenderDriver::set_viewport(
    const CommandBuffer *command_buffer,
    const float x,
    const float y,
    const float width,
    const float height,
    const float min_depth,
    const float max_depth) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);

    const VkViewport viewport = {
        .x = x,
        .y = y,
        .width = width,
        .height = height,
        .minDepth = min_depth,
        .maxDepth = max_depth,
    };
    vkCmdSetViewport(vulkan_command_buffer->command_buffer, 0, 1, &viewport);
}

void VulkanRenderDriver::set_scissor(
    const CommandBuffer *command_buffer,
    const int32_t x,
    const int32_t y,
    const uint32_t width,
    const uint32_t height) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);

    const VkRect2D scissor = {
        .offset =
            {
                .x = x,
                .y = y,
            },
        .extent =
            {
                .width = width,
                .height = height,
            },
    };
    vkCmdSetScissor(vulkan_command_buffer->command_buffer, 0, 1, &scissor);
}

void VulkanRenderDriver::draw(
    const CommandBuffer *command_buffer,
    const uint32_t vertex_count,
    const uint32_t instance_count,
    const uint32_t first_vertex,
    const uint32_t first_instance) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    vkCmdDraw(vulkan_command_buffer->command_buffer, vertex_count, instance_count, first_vertex, first_instance);
}

void VulkanRenderDriver::draw_indexed(
    const CommandBuffer *command_buffer,
    const uint32_t index_count,
    const uint32_t instance_count,
    const uint32_t first_index,
    const int32_t vertex_offset,
    const uint32_t first_instance) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    vkCmdDrawIndexed(vulkan_command_buffer->command_buffer, index_count, instance_count, first_index, vertex_offset, first_instance);
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

void VulkanRenderDriver::create_surface(const WindowSystem &window_system, const WindowId id)
{
    SDL_Window *window = window_system.get_native_window(id);

    HE_ASSERT(SDL_Vulkan_CreateSurface(window, m_instance, nullptr, &m_surface));
    HE_ASSERT(m_surface != VK_NULL_HANDLE);
}

void VulkanRenderDriver::create_swapchain(const uint32_t width, const uint32_t height)
{
    VkSurfaceCapabilitiesKHR surface_capabilities = {};
    HE_VK_CHECK(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physical_device, m_surface, &surface_capabilities),
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR);

    const VkExtent2D surface_extent = choose_extent(width, height, surface_capabilities);
    m_swapchain_width = surface_extent.width;
    m_swapchain_height = surface_extent.height;

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
        const Format format = format_to_texture_format(surface_format.format);
        const VkImageView image_view = create_internal_image_view(image, Dimension::Texture2D, format);

        Texture *texture = new VulkanTexture({
            .image = image,
            .allocation = VK_NULL_HANDLE,
            .image_view = image_view,
        });

        texture->label = std::nullopt;
        texture->width = surface_extent.width;
        texture->height = surface_extent.height;
        texture->depth = 1;
        texture->array_size = 1;
        texture->mip_levels = 1;
        texture->format = format;
        texture->dimension = Dimension::Texture2D;
        texture->usage = TextureUsage::RenderAttachment;

        m_swapchain_textures.push_back(texture);
    }
}

void VulkanRenderDriver::recreate_swapchain()
{
    destroy_swapchain();
    create_swapchain(m_swapchain_width, m_swapchain_height);

    m_swapchain_out_of_date = false;
}

void VulkanRenderDriver::destroy_swapchain()
{
    m_swapchain_textures.clear();

    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
}

// FIXME: Check if the window id matches and maybe save the width/height
void VulkanRenderDriver::on_resize(const WindowResizeEvent &event)
{
    m_swapchain_width = event.width();
    m_swapchain_height = event.height();
    m_swapchain_out_of_date = true;
}

void VulkanRenderDriver::find_descriptor_counts()
{
    VkPhysicalDeviceProperties properties = {};
    vkGetPhysicalDeviceProperties(m_physical_device, &properties);

    for (size_t index = 0; index != s_descriptor_count; ++index)
    {
        const VkDescriptorType &descriptor_type = s_descriptor_types[index];

        const uint32_t limit = [&properties, &descriptor_type]()
        {
            switch (descriptor_type)
            {
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
                return properties.limits.maxDescriptorSetStorageBuffers;
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
                return properties.limits.maxDescriptorSetSampledImages;
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
                return properties.limits.maxDescriptorSetStorageImages;
            case VK_DESCRIPTOR_TYPE_SAMPLER:
                return properties.limits.maxDescriptorSetSamplers;
            default:
                HE_UNREACHABLE();
            }
        }();

        const uint32_t descriptor_count = limit > s_descriptor_limit ? s_descriptor_limit : limit;
        m_descriptor_counts[index] = descriptor_count;
    }
}

void VulkanRenderDriver::create_descriptor_pool()
{
    std::array<VkDescriptorPoolSize, s_descriptor_count> descriptor_pool_sizes = {};
    for (size_t index = 0; index < s_descriptor_count; index++)
    {
        const VkDescriptorType &descriptor_type = s_descriptor_types[index];
        const uint32_t descriptor_count = m_descriptor_counts[index];

        const VkDescriptorPoolSize descriptor_pool_size = {
            .type = descriptor_type,
            .descriptorCount = descriptor_count,
        };

        descriptor_pool_sizes[index] = descriptor_pool_size;
    }

    const VkDescriptorPoolCreateInfo descriptor_pool_create_info = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = static_cast<uint32_t>(s_descriptor_count),
        .poolSizeCount = static_cast<uint32_t>(descriptor_pool_sizes.size()),
        .pPoolSizes = descriptor_pool_sizes.data(),
    };

    HE_VK_CHECK(vkCreateDescriptorPool(m_device, &descriptor_pool_create_info, nullptr, &m_descriptor_pool), vkCreateDescriptorPool);
    HE_ASSERT(m_descriptor_pool != VK_NULL_HANDLE);
}

void VulkanRenderDriver::create_descriptor_set_layouts()
{
    for (size_t index = 0; index != s_descriptor_count; ++index)
    {
        const VkDescriptorType &descriptor_type = s_descriptor_types[index];
        const uint32_t descriptor_count = m_descriptor_counts[index];

        const VkDescriptorSetLayoutBinding descriptor_set_layout_binding = {
            .binding = 0,
            .descriptorType = descriptor_type,
            .descriptorCount = descriptor_count,
            .stageFlags = VK_SHADER_STAGE_ALL,
            .pImmutableSamplers = nullptr,
        };

        constexpr VkDescriptorBindingFlags descriptor_binding_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT |
                                                                      VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT |
                                                                      VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

        VkDescriptorSetLayoutBindingFlagsCreateInfo descriptor_set_layout_binding_flags_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
            .pNext = nullptr,
            .bindingCount = 1,
            .pBindingFlags = &descriptor_binding_flags,
        };

        const VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
            .pNext = &descriptor_set_layout_binding_flags_info,
            .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
            .bindingCount = 1,
            .pBindings = &descriptor_set_layout_binding,
        };

        HE_VK_CHECK(
            vkCreateDescriptorSetLayout(m_device, &descriptor_set_layout_create_info, nullptr, &m_descriptor_set_layouts[index]),
            vkCreateDescriptorSetLayout);
        HE_ASSERT(m_descriptor_set_layouts[index] != VK_NULL_HANDLE);
    }
}

void VulkanRenderDriver::create_descriptor_sets()
{
    for (size_t index = 0; index != m_descriptor_set_layouts.size(); ++index)
    {
        const VkDescriptorSetLayout &descriptor_set_layout = m_descriptor_set_layouts[index];
        const uint32_t descriptor_count = m_descriptor_counts[index];

        VkDescriptorSetVariableDescriptorCountAllocateInfo descriptor_set_variable_descriptor_count_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorSetCount = 1,
            .pDescriptorCounts = &descriptor_count,
        };

        const VkDescriptorSetAllocateInfo descriptor_set_allocate_info = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = &descriptor_set_variable_descriptor_count_info,
            .descriptorPool = m_descriptor_pool,
            .descriptorSetCount = 1,
            .pSetLayouts = &descriptor_set_layout,
        };

        HE_VK_CHECK(vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, &m_descriptor_sets[index]), vkAllocateDescriptorSets);
        HE_ASSERT(m_descriptor_sets[index] != VK_NULL_HANDLE);
    }
}

VkImageView VulkanRenderDriver::create_internal_image_view(const VkImage image, const Dimension dimension, const Format format) const
{
    constexpr VkComponentMapping component_mapping = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };

    // FIXME: Add subresource
    const VkImageSubresourceRange subresource_range = {
        .aspectMask = get_image_aspect_flags(format),
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
        .viewType = get_image_view_type(dimension),
        .format = get_format(format),
        .components = component_mapping,
        .subresourceRange = subresource_range,
    };

    VkImageView image_view = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateImageView(m_device, &image_view_create_info, nullptr, &image_view), vkCreateImageView);
    HE_ASSERT(image_view != VK_NULL_HANDLE);

    return image_view;
}

void VulkanRenderDriver::transition_texture_layout(const CommandBuffer *command_buffer, Texture *texture, const VkImageLayout new_layout)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    VulkanTexture *vulkan_texture = reinterpret_cast<VulkanTexture *>(texture);

    // FIXME: Add more specific src/dst stage/access masks

    // FIXME: Add Subresource
    const VkImageSubresourceRange subresource_range = {
        .aspectMask = get_image_aspect_flags(vulkan_texture->format),
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

VkBufferUsageFlags VulkanRenderDriver::get_buffer_usage_flags(const BitFlags<BufferUsage> buffer_usage_flags)
{
    VkBufferUsageFlags usage_flags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    if (buffer_usage_flags & BufferUsage::Index)
    {
        usage_flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }

    if (buffer_usage_flags & BufferUsage::Indirect)
    {
        usage_flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    }

    if (buffer_usage_flags & BufferUsage::Storage)
    {
        usage_flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    return usage_flags;
}

VkFilter VulkanRenderDriver::get_filter(const Filter filter)
{
    switch (filter)
    {
    case Filter::Nearest:
        return VK_FILTER_NEAREST;
    case Filter::Linear:
        return VK_FILTER_LINEAR;
    default:
        HE_UNREACHABLE();
    }
}

VkSamplerMipmapMode VulkanRenderDriver::get_sampler_mipmap_mode(const Filter filter)
{
    switch (filter)
    {
    case Filter::Nearest:
        return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case Filter::Linear:
        return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    default:
        HE_UNREACHABLE();
    }
}

VkSamplerAddressMode VulkanRenderDriver::get_sampler_address_mode(const AddressMode filter)
{
    switch (filter)
    {
    case AddressMode::Repeat:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case AddressMode::MirroredRepeat:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case AddressMode::ClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case AddressMode::ClampToBorder:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case AddressMode::MirrorClampToEdge:
        return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    default:
        HE_UNREACHABLE();
    }
}

VkBorderColor VulkanRenderDriver::get_border_color(const BorderColor border_color)
{
    switch (border_color)
    {
    case BorderColor::TransparentBlack:
        return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    case BorderColor::OpaqueBlack:
        return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    case BorderColor::OpaqueWhite:
        return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    default:
        HE_UNREACHABLE();
    }
}

Format VulkanRenderDriver::format_to_texture_format(const VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_UNDEFINED:
        return Format::Unknown;
    case VK_FORMAT_R8_UNORM:
        return Format::R8Unorm;
    case VK_FORMAT_R8_SNORM:
        return Format::R8Snorm;
    case VK_FORMAT_R8_UINT:
        return Format::R8Uint;
    case VK_FORMAT_R8_SINT:
        return Format::R8Sint;
    case VK_FORMAT_R8_SRGB:
        return Format::R8Srgb;
    case VK_FORMAT_R8G8_UNORM:
        return Format::Rg8Unorm;
    case VK_FORMAT_R8G8_SNORM:
        return Format::Rg8Snorm;
    case VK_FORMAT_R8G8_UINT:
        return Format::Rg8Uint;
    case VK_FORMAT_R8G8_SINT:
        return Format::Rg8Sint;
    case VK_FORMAT_R8G8_SRGB:
        return Format::Rg8Srgb;
    case VK_FORMAT_R8G8B8_UNORM:
        return Format::Rgb8Unorm;
    case VK_FORMAT_R8G8B8_SNORM:
        return Format::Rgb8Snorm;
    case VK_FORMAT_R8G8B8_UINT:
        return Format::Rgb8Uint;
    case VK_FORMAT_R8G8B8_SINT:
        return Format::Rgb8Sint;
    case VK_FORMAT_R8G8B8_SRGB:
        return Format::Rgb8Srgb;
    case VK_FORMAT_B8G8R8_UNORM:
        return Format::Bgr8Unorm;
    case VK_FORMAT_B8G8R8_SNORM:
        return Format::Bgr8Snorm;
    case VK_FORMAT_B8G8R8_UINT:
        return Format::Bgr8Uint;
    case VK_FORMAT_B8G8R8_SINT:
        return Format::Bgr8Sint;
    case VK_FORMAT_B8G8R8_SRGB:
        return Format::Bgr8Srgb;
    case VK_FORMAT_R8G8B8A8_UNORM:
        return Format::Rgba8Unorm;
    case VK_FORMAT_R8G8B8A8_SNORM:
        return Format::Rgba8Snorm;
    case VK_FORMAT_R8G8B8A8_UINT:
        return Format::Rgba8Uint;
    case VK_FORMAT_R8G8B8A8_SINT:
        return Format::Rgba8Sint;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return Format::Rgba8Srgb;
    case VK_FORMAT_B8G8R8A8_UNORM:
        return Format::Bgra8Unorm;
    case VK_FORMAT_B8G8R8A8_SNORM:
        return Format::Bgra8Snorm;
    case VK_FORMAT_B8G8R8A8_UINT:
        return Format::Bgra8Uint;
    case VK_FORMAT_B8G8R8A8_SINT:
        return Format::Bgra8Sint;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return Format::Bgra8Srgb;
    case VK_FORMAT_R16_UNORM:
        return Format::R16Unorm;
    case VK_FORMAT_R16_SNORM:
        return Format::R16Snorm;
    case VK_FORMAT_R16_UINT:
        return Format::R16Uint;
    case VK_FORMAT_R16_SINT:
        return Format::R16Sint;
    case VK_FORMAT_R16_SFLOAT:
        return Format::R16Sfloat;
    case VK_FORMAT_R16G16_UNORM:
        return Format::Rg16Unorm;
    case VK_FORMAT_R16G16_SNORM:
        return Format::Rg16Snorm;
    case VK_FORMAT_R16G16_UINT:
        return Format::Rg16Uint;
    case VK_FORMAT_R16G16_SINT:
        return Format::Rg16Sint;
    case VK_FORMAT_R16G16_SFLOAT:
        return Format::Rg16Sfloat;
    case VK_FORMAT_R16G16B16_UNORM:
        return Format::Rgb16Unorm;
    case VK_FORMAT_R16G16B16_SNORM:
        return Format::Rgb16Snorm;
    case VK_FORMAT_R16G16B16_UINT:
        return Format::Rgb16Uint;
    case VK_FORMAT_R16G16B16_SINT:
        return Format::Rgb16Sint;
    case VK_FORMAT_R16G16B16_SFLOAT:
        return Format::Rgb16Sfloat;
    case VK_FORMAT_R16G16B16A16_UNORM:
        return Format::Rgba16Unorm;
    case VK_FORMAT_R16G16B16A16_SNORM:
        return Format::Rgba16Snorm;
    case VK_FORMAT_R16G16B16A16_UINT:
        return Format::Rgba16Uint;
    case VK_FORMAT_R16G16B16A16_SINT:
        return Format::Rgba16Sint;
    case VK_FORMAT_R16G16B16A16_SFLOAT:
        return Format::Rgba16Sfloat;
    case VK_FORMAT_R32_UINT:
        return Format::R32Uint;
    case VK_FORMAT_R32_SINT:
        return Format::R32Sint;
    case VK_FORMAT_R32_SFLOAT:
        return Format::R32Sfloat;
    case VK_FORMAT_R32G32_UINT:
        return Format::Rg32Uint;
    case VK_FORMAT_R32G32_SINT:
        return Format::Rg32Sint;
    case VK_FORMAT_R32G32_SFLOAT:
        return Format::Rg32Sfloat;
    case VK_FORMAT_R32G32B32_UINT:
        return Format::Rgb32Uint;
    case VK_FORMAT_R32G32B32_SINT:
        return Format::Rgb32Sint;
    case VK_FORMAT_R32G32B32_SFLOAT:
        return Format::Rgb32Sfloat;
    case VK_FORMAT_R32G32B32A32_UINT:
        return Format::Rgba32Uint;
    case VK_FORMAT_R32G32B32A32_SINT:
        return Format::Rgba32Sint;
    case VK_FORMAT_R32G32B32A32_SFLOAT:
        return Format::Rgba32Sfloat;
    case VK_FORMAT_R64_UINT:
        return Format::R64Uint;
    case VK_FORMAT_R64_SINT:
        return Format::R64Sint;
    case VK_FORMAT_R64_SFLOAT:
        return Format::R64Sfloat;
    case VK_FORMAT_R64G64_UINT:
        return Format::Rg64Uint;
    case VK_FORMAT_R64G64_SINT:
        return Format::Rg64Sint;
    case VK_FORMAT_R64G64_SFLOAT:
        return Format::Rg64Sfloat;
    case VK_FORMAT_R64G64B64_UINT:
        return Format::Rgb64Uint;
    case VK_FORMAT_R64G64B64_SINT:
        return Format::Rgb64Sint;
    case VK_FORMAT_R64G64B64_SFLOAT:
        return Format::Rgb64Sfloat;
    case VK_FORMAT_R64G64B64A64_UINT:
        return Format::Rgba64Uint;
    case VK_FORMAT_R64G64B64A64_SINT:
        return Format::Rgba64Sint;
    case VK_FORMAT_R64G64B64A64_SFLOAT:
        return Format::Rgba64Sfloat;
    case VK_FORMAT_D16_UNORM:
        return Format::D16Unorm;
    case VK_FORMAT_D32_SFLOAT:
        return Format::D32Sfloat;
    case VK_FORMAT_S8_UINT:
        return Format::S8Uint;
    case VK_FORMAT_D16_UNORM_S8_UINT:
        return Format::D16UnormS8Uint;
    case VK_FORMAT_D24_UNORM_S8_UINT:
        return Format::D24UnormS8Uint;
    case VK_FORMAT_D32_SFLOAT_S8_UINT:
        return Format::D32SfloatS8Uint;
    default:
        HE_UNREACHABLE();
    }
}

VkFormat VulkanRenderDriver::get_format(const Format format)
{
    switch (format)
    {
    case Format::Unknown:
        return VK_FORMAT_UNDEFINED;
    case Format::R8Unorm:
        return VK_FORMAT_R8_UNORM;
    case Format::R8Snorm:
        return VK_FORMAT_R8_SNORM;
    case Format::R8Uint:
        return VK_FORMAT_R8_UINT;
    case Format::R8Sint:
        return VK_FORMAT_R8_SINT;
    case Format::R8Srgb:
        return VK_FORMAT_R8_SRGB;
    case Format::Rg8Unorm:
        return VK_FORMAT_R8G8_UNORM;
    case Format::Rg8Snorm:
        return VK_FORMAT_R8G8_SNORM;
    case Format::Rg8Uint:
        return VK_FORMAT_R8G8_UINT;
    case Format::Rg8Sint:
        return VK_FORMAT_R8G8_SINT;
    case Format::Rg8Srgb:
        return VK_FORMAT_R8G8_SRGB;
    case Format::Rgb8Unorm:
        return VK_FORMAT_R8G8B8_UNORM;
    case Format::Rgb8Snorm:
        return VK_FORMAT_R8G8B8_SNORM;
    case Format::Rgb8Uint:
        return VK_FORMAT_R8G8B8_UINT;
    case Format::Rgb8Sint:
        return VK_FORMAT_R8G8B8_SINT;
    case Format::Rgb8Srgb:
        return VK_FORMAT_R8G8B8_SRGB;
    case Format::Bgr8Unorm:
        return VK_FORMAT_B8G8R8_UNORM;
    case Format::Bgr8Snorm:
        return VK_FORMAT_B8G8R8_SNORM;
    case Format::Bgr8Uint:
        return VK_FORMAT_B8G8R8_UINT;
    case Format::Bgr8Sint:
        return VK_FORMAT_B8G8R8_SINT;
    case Format::Bgr8Srgb:
        return VK_FORMAT_B8G8R8_SRGB;
    case Format::Rgba8Unorm:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case Format::Rgba8Snorm:
        return VK_FORMAT_R8G8B8A8_SNORM;
    case Format::Rgba8Uint:
        return VK_FORMAT_R8G8B8A8_UINT;
    case Format::Rgba8Sint:
        return VK_FORMAT_R8G8B8A8_SINT;
    case Format::Rgba8Srgb:
        return VK_FORMAT_R8G8B8A8_SRGB;
    case Format::Bgra8Unorm:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case Format::Bgra8Snorm:
        return VK_FORMAT_B8G8R8A8_SNORM;
    case Format::Bgra8Uint:
        return VK_FORMAT_B8G8R8A8_UINT;
    case Format::Bgra8Sint:
        return VK_FORMAT_B8G8R8A8_SINT;
    case Format::Bgra8Srgb:
        return VK_FORMAT_B8G8R8A8_SRGB;
    case Format::R16Unorm:
        return VK_FORMAT_R16_UNORM;
    case Format::R16Snorm:
        return VK_FORMAT_R16_SNORM;
    case Format::R16Uint:
        return VK_FORMAT_R16_UINT;
    case Format::R16Sint:
        return VK_FORMAT_R16_SINT;
    case Format::R16Sfloat:
        return VK_FORMAT_R16_SFLOAT;
    case Format::Rg16Unorm:
        return VK_FORMAT_R16G16_UNORM;
    case Format::Rg16Snorm:
        return VK_FORMAT_R16G16_SNORM;
    case Format::Rg16Uint:
        return VK_FORMAT_R16G16_UINT;
    case Format::Rg16Sint:
        return VK_FORMAT_R16G16_SINT;
    case Format::Rg16Sfloat:
        return VK_FORMAT_R16G16_SFLOAT;
    case Format::Rgb16Unorm:
        return VK_FORMAT_R16G16B16_UNORM;
    case Format::Rgb16Snorm:
        return VK_FORMAT_R16G16B16_SNORM;
    case Format::Rgb16Uint:
        return VK_FORMAT_R16G16B16_UINT;
    case Format::Rgb16Sint:
        return VK_FORMAT_R16G16B16_SINT;
    case Format::Rgb16Sfloat:
        return VK_FORMAT_R16G16B16_SFLOAT;
    case Format::Rgba16Unorm:
        return VK_FORMAT_R16G16B16A16_UNORM;
    case Format::Rgba16Snorm:
        return VK_FORMAT_R16G16B16A16_SNORM;
    case Format::Rgba16Uint:
        return VK_FORMAT_R16G16B16A16_UINT;
    case Format::Rgba16Sint:
        return VK_FORMAT_R16G16B16A16_SINT;
    case Format::Rgba16Sfloat:
        return VK_FORMAT_R16G16B16A16_SFLOAT;
    case Format::R32Uint:
        return VK_FORMAT_R32_UINT;
    case Format::R32Sint:
        return VK_FORMAT_R32_SINT;
    case Format::R32Sfloat:
        return VK_FORMAT_R32_SFLOAT;
    case Format::Rg32Uint:
        return VK_FORMAT_R32G32_UINT;
    case Format::Rg32Sint:
        return VK_FORMAT_R32G32_SINT;
    case Format::Rg32Sfloat:
        return VK_FORMAT_R32G32_SFLOAT;
    case Format::Rgb32Uint:
        return VK_FORMAT_R32G32B32_UINT;
    case Format::Rgb32Sint:
        return VK_FORMAT_R32G32B32_SINT;
    case Format::Rgb32Sfloat:
        return VK_FORMAT_R32G32B32_SFLOAT;
    case Format::Rgba32Uint:
        return VK_FORMAT_R32G32B32A32_UINT;
    case Format::Rgba32Sint:
        return VK_FORMAT_R32G32B32A32_SINT;
    case Format::Rgba32Sfloat:
        return VK_FORMAT_R32G32B32A32_SFLOAT;
    case Format::R64Uint:
        return VK_FORMAT_R64_UINT;
    case Format::R64Sint:
        return VK_FORMAT_R64_SINT;
    case Format::R64Sfloat:
        return VK_FORMAT_R64_SFLOAT;
    case Format::Rg64Uint:
        return VK_FORMAT_R64G64_UINT;
    case Format::Rg64Sint:
        return VK_FORMAT_R64G64_SINT;
    case Format::Rg64Sfloat:
        return VK_FORMAT_R64G64_SFLOAT;
    case Format::Rgb64Uint:
        return VK_FORMAT_R64G64B64_UINT;
    case Format::Rgb64Sint:
        return VK_FORMAT_R64G64B64_SINT;
    case Format::Rgb64Sfloat:
        return VK_FORMAT_R64G64B64_SFLOAT;
    case Format::Rgba64Uint:
        return VK_FORMAT_R64G64B64A64_UINT;
    case Format::Rgba64Sint:
        return VK_FORMAT_R64G64B64A64_SINT;
    case Format::Rgba64Sfloat:
        return VK_FORMAT_R64G64B64A64_SFLOAT;
    case Format::D16Unorm:
        return VK_FORMAT_D16_UNORM;
    case Format::D32Sfloat:
        return VK_FORMAT_D32_SFLOAT;
    case Format::S8Uint:
        return VK_FORMAT_S8_UINT;
    case Format::D16UnormS8Uint:
        return VK_FORMAT_D16_UNORM_S8_UINT;
    case Format::D24UnormS8Uint:
        return VK_FORMAT_D24_UNORM_S8_UINT;
    case Format::D32SfloatS8Uint:
        return VK_FORMAT_D32_SFLOAT_S8_UINT;
    default:
        HE_UNREACHABLE();
    }
}

VkImageType VulkanRenderDriver::get_image_type(const Dimension dimension)
{
    switch (dimension)
    {
    case Dimension::Texture1D:
    case Dimension::Texture1DArray:
        return VK_IMAGE_TYPE_1D;
    case Dimension::Texture2D:
    case Dimension::Texture2DArray:
        return VK_IMAGE_TYPE_2D;
    case Dimension::Texture3D:
        return VK_IMAGE_TYPE_3D;
    case Dimension::Unknown:
    default:
        HE_UNREACHABLE();
    }
}

VkImageUsageFlags VulkanRenderDriver::get_image_usage_flags(const BitFlags<TextureUsage> texture_usage_flags, const Format format)
{
    VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    if (texture_usage_flags & TextureUsage::Storage)
    {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }
    else
    {
        usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }

    if (texture_usage_flags & TextureUsage::RenderAttachment)
    {
        switch (format)
        {
        case Format::R8Unorm:
        case Format::R8Snorm:
        case Format::R8Uint:
        case Format::R8Sint:
        case Format::R8Srgb:
        case Format::Rg8Unorm:
        case Format::Rg8Snorm:
        case Format::Rg8Uint:
        case Format::Rg8Sint:
        case Format::Rg8Srgb:
        case Format::Rgb8Unorm:
        case Format::Rgb8Snorm:
        case Format::Rgb8Uint:
        case Format::Rgb8Sint:
        case Format::Rgb8Srgb:
        case Format::Bgr8Unorm:
        case Format::Bgr8Snorm:
        case Format::Bgr8Uint:
        case Format::Bgr8Sint:
        case Format::Bgr8Srgb:
        case Format::Rgba8Unorm:
        case Format::Rgba8Snorm:
        case Format::Rgba8Uint:
        case Format::Rgba8Sint:
        case Format::Rgba8Srgb:
        case Format::Bgra8Unorm:
        case Format::Bgra8Snorm:
        case Format::Bgra8Uint:
        case Format::Bgra8Sint:
        case Format::Bgra8Srgb:
        case Format::R16Unorm:
        case Format::R16Snorm:
        case Format::R16Uint:
        case Format::R16Sint:
        case Format::R16Sfloat:
        case Format::Rg16Unorm:
        case Format::Rg16Snorm:
        case Format::Rg16Uint:
        case Format::Rg16Sint:
        case Format::Rg16Sfloat:
        case Format::Rgb16Unorm:
        case Format::Rgb16Snorm:
        case Format::Rgb16Uint:
        case Format::Rgb16Sint:
        case Format::Rgb16Sfloat:
        case Format::Rgba16Unorm:
        case Format::Rgba16Snorm:
        case Format::Rgba16Uint:
        case Format::Rgba16Sint:
        case Format::Rgba16Sfloat:
        case Format::R32Uint:
        case Format::R32Sint:
        case Format::R32Sfloat:
        case Format::Rg32Uint:
        case Format::Rg32Sint:
        case Format::Rg32Sfloat:
        case Format::Rgb32Uint:
        case Format::Rgb32Sint:
        case Format::Rgb32Sfloat:
        case Format::Rgba32Uint:
        case Format::Rgba32Sint:
        case Format::Rgba32Sfloat:
        case Format::R64Uint:
        case Format::R64Sint:
        case Format::R64Sfloat:
        case Format::Rg64Uint:
        case Format::Rg64Sint:
        case Format::Rg64Sfloat:
        case Format::Rgb64Uint:
        case Format::Rgb64Sint:
        case Format::Rgb64Sfloat:
        case Format::Rgba64Uint:
        case Format::Rgba64Sint:
        case Format::Rgba64Sfloat:
            usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            break;
        case Format::D16Unorm:
        case Format::D32Sfloat:
        case Format::S8Uint:
        case Format::D16UnormS8Uint:
        case Format::D24UnormS8Uint:
        case Format::D32SfloatS8Uint:
            usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
            break;
        case Format::Unknown:
        default:
            HE_UNREACHABLE();
        }
    }

    return usage;
}

VkImageAspectFlags VulkanRenderDriver::get_image_aspect_flags(const Format format)
{
    switch (format)
    {
    case Format::R8Unorm:
    case Format::R8Snorm:
    case Format::R8Uint:
    case Format::R8Sint:
    case Format::R8Srgb:
    case Format::Rg8Unorm:
    case Format::Rg8Snorm:
    case Format::Rg8Uint:
    case Format::Rg8Sint:
    case Format::Rg8Srgb:
    case Format::Rgb8Unorm:
    case Format::Rgb8Snorm:
    case Format::Rgb8Uint:
    case Format::Rgb8Sint:
    case Format::Rgb8Srgb:
    case Format::Bgr8Unorm:
    case Format::Bgr8Snorm:
    case Format::Bgr8Uint:
    case Format::Bgr8Sint:
    case Format::Bgr8Srgb:
    case Format::Rgba8Unorm:
    case Format::Rgba8Snorm:
    case Format::Rgba8Uint:
    case Format::Rgba8Sint:
    case Format::Rgba8Srgb:
    case Format::Bgra8Unorm:
    case Format::Bgra8Snorm:
    case Format::Bgra8Uint:
    case Format::Bgra8Sint:
    case Format::Bgra8Srgb:
    case Format::R16Unorm:
    case Format::R16Snorm:
    case Format::R16Uint:
    case Format::R16Sint:
    case Format::R16Sfloat:
    case Format::Rg16Unorm:
    case Format::Rg16Snorm:
    case Format::Rg16Uint:
    case Format::Rg16Sint:
    case Format::Rg16Sfloat:
    case Format::Rgb16Unorm:
    case Format::Rgb16Snorm:
    case Format::Rgb16Uint:
    case Format::Rgb16Sint:
    case Format::Rgb16Sfloat:
    case Format::Rgba16Unorm:
    case Format::Rgba16Snorm:
    case Format::Rgba16Uint:
    case Format::Rgba16Sint:
    case Format::Rgba16Sfloat:
    case Format::R32Uint:
    case Format::R32Sint:
    case Format::R32Sfloat:
    case Format::Rg32Uint:
    case Format::Rg32Sint:
    case Format::Rg32Sfloat:
    case Format::Rgb32Uint:
    case Format::Rgb32Sint:
    case Format::Rgb32Sfloat:
    case Format::Rgba32Uint:
    case Format::Rgba32Sint:
    case Format::Rgba32Sfloat:
    case Format::R64Uint:
    case Format::R64Sint:
    case Format::R64Sfloat:
    case Format::Rg64Uint:
    case Format::Rg64Sint:
    case Format::Rg64Sfloat:
    case Format::Rgb64Uint:
    case Format::Rgb64Sint:
    case Format::Rgb64Sfloat:
    case Format::Rgba64Uint:
    case Format::Rgba64Sint:
    case Format::Rgba64Sfloat:
        return VK_IMAGE_ASPECT_COLOR_BIT;
    case Format::D16Unorm:
    case Format::D32Sfloat:
        return VK_IMAGE_ASPECT_DEPTH_BIT;
    case Format::S8Uint:
        return VK_IMAGE_ASPECT_STENCIL_BIT;
    case Format::D16UnormS8Uint:
    case Format::D24UnormS8Uint:
    case Format::D32SfloatS8Uint:
        return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    case Format::Unknown:
    default:
        HE_UNREACHABLE();
    }
}

VkImageViewType VulkanRenderDriver::get_image_view_type(const Dimension dimension)
{
    switch (dimension)
    {
    case Dimension::Texture1D:
        return VK_IMAGE_VIEW_TYPE_1D;
    case Dimension::Texture1DArray:
        return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    case Dimension::Texture2D:
        return VK_IMAGE_VIEW_TYPE_2D;
    case Dimension::Texture2DArray:
        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case Dimension::Texture3D:
        return VK_IMAGE_VIEW_TYPE_3D;
    case Dimension::Unknown:
    default:
        HE_UNREACHABLE();
    }
}

VkPrimitiveTopology VulkanRenderDriver::get_primitive_topology(const PrimitiveTopology primitive_topology)
{
    switch (primitive_topology)
    {
    case PrimitiveTopology::PointList:
        return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case PrimitiveTopology::LineList:
        return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case PrimitiveTopology::LineStrip:
        return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case PrimitiveTopology::TriangleList:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case PrimitiveTopology::TriangleStrip:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case PrimitiveTopology::TriangleFan:
        return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    default:
        HE_UNREACHABLE();
    }
}

VkPolygonMode VulkanRenderDriver::get_polygon_mode(const PolygonMode polygon_mode)
{
    switch (polygon_mode)
    {
    case PolygonMode::Fill:
        return VK_POLYGON_MODE_FILL;
    case PolygonMode::Line:
        return VK_POLYGON_MODE_LINE;
    case PolygonMode::Point:
        return VK_POLYGON_MODE_POINT;
    default:
        HE_UNREACHABLE();
    }
}

VkCullModeFlags VulkanRenderDriver::get_cull_mode_flags(const Face face)
{
    switch (face)
    {
    case Face::None:
        return VK_CULL_MODE_NONE;
    case Face::Front:
        return VK_CULL_MODE_FRONT_BIT;
    case Face::Back:
        return VK_CULL_MODE_BACK_BIT;
    default:
        HE_UNREACHABLE();
    }
}

VkFrontFace VulkanRenderDriver::get_front_face(const FrontFace front_face)
{
    switch (front_face)
    {
    case FrontFace::CounterClockwise:
        return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    case FrontFace::Clockwise:
        return VK_FRONT_FACE_CLOCKWISE;
    default:
        HE_UNREACHABLE();
    }
}

VkCompareOp VulkanRenderDriver::get_compare_operation(const CompareOperation compare_operation)
{
    switch (compare_operation)
    {
    case CompareOperation::Never:
        return VK_COMPARE_OP_NEVER;
    case CompareOperation::Less:
        return VK_COMPARE_OP_LESS;
    case CompareOperation::Equal:
        return VK_COMPARE_OP_EQUAL;
    case CompareOperation::LessEqual:
        return VK_COMPARE_OP_LESS_OR_EQUAL;
    case CompareOperation::Greater:
        return VK_COMPARE_OP_GREATER;
    case CompareOperation::NotEqual:
        return VK_COMPARE_OP_NOT_EQUAL;
    case CompareOperation::GreaterEqual:
        return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case CompareOperation::Always:
        return VK_COMPARE_OP_ALWAYS;
    default:
        HE_UNREACHABLE();
    }
}

VkBlendFactor VulkanRenderDriver::get_blend_factor(const BlendFactor blend_factor)
{
    switch (blend_factor)
    {
    case BlendFactor::Zero:
        return VK_BLEND_FACTOR_ZERO;
    case BlendFactor::One:
        return VK_BLEND_FACTOR_ONE;
    case BlendFactor::SrcColor:
        return VK_BLEND_FACTOR_SRC_COLOR;
    case BlendFactor::OneMinusSrcColor:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case BlendFactor::DstColor:
        return VK_BLEND_FACTOR_DST_COLOR;
    case BlendFactor::OneMinusDstColor:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case BlendFactor::SrcAlpha:
        return VK_BLEND_FACTOR_SRC_ALPHA;
    case BlendFactor::OneMinusSrcAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DstAlpha:
        return VK_BLEND_FACTOR_DST_ALPHA;
    case BlendFactor::OneMinusDstAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case BlendFactor::ConstantColor:
        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case BlendFactor::OneMinusConstantColor:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::ConstantAlpha:
        return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case BlendFactor::OneMinusConstantAlpha:
        return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    case BlendFactor::SrcAlphaSaturate:
        return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case BlendFactor::Src1Color:
        return VK_BLEND_FACTOR_SRC1_COLOR;
    case BlendFactor::OneMinusSrc1Color:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case BlendFactor::Src1Alpha:
        return VK_BLEND_FACTOR_SRC1_ALPHA;
    case BlendFactor::OneMinusSrc1Alpha:
        return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    default:
        HE_UNREACHABLE();
    }
}

VkBlendOp VulkanRenderDriver::get_blend_operation(const BlendOperation blend_operation)
{
    switch (blend_operation)
    {
    case BlendOperation::Add:
        return VK_BLEND_OP_ADD;
    case BlendOperation::Subtract:
        return VK_BLEND_OP_SUBTRACT;
    case BlendOperation::ReverseSubtract:
        return VK_BLEND_OP_REVERSE_SUBTRACT;
    case BlendOperation::Min:
        return VK_BLEND_OP_MIN;
    case BlendOperation::Max:
        return VK_BLEND_OP_MAX;
    default:
        HE_UNREACHABLE();
    }
}

VkColorComponentFlags VulkanRenderDriver::get_color_component_flags(const BitFlags<ColorWrites> color_writes)
{
    VkColorComponentFlags color_component_flags = 0;
    if (color_writes & ColorWrites::R)
    {
        color_component_flags |= VK_COLOR_COMPONENT_R_BIT;
    }

    if (color_writes & ColorWrites::G)
    {
        color_component_flags |= VK_COLOR_COMPONENT_G_BIT;
    }

    if (color_writes & ColorWrites::B)
    {
        color_component_flags |= VK_COLOR_COMPONENT_B_BIT;
    }

    if (color_writes & ColorWrites::A)
    {
        color_component_flags |= VK_COLOR_COMPONENT_A_BIT;
    }

    return color_component_flags;
}

VkAttachmentLoadOp VulkanRenderDriver::get_attachment_load_operation(LoadOperation load_operation)
{
    switch (load_operation)
    {
    case LoadOperation::Clear:
        return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case LoadOperation::Load:
        return VK_ATTACHMENT_LOAD_OP_LOAD;
    case LoadOperation::DontCare:
        return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    default:
        HE_UNREACHABLE();
    }
}

VkAttachmentStoreOp VulkanRenderDriver::get_attachment_store_operation(StoreOperation store_operation)
{
    switch (store_operation)
    {
    case StoreOperation::Store:
        return VK_ATTACHMENT_STORE_OP_STORE;
    case StoreOperation::DontCare:
        return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    default:
        HE_UNREACHABLE();
    }
}