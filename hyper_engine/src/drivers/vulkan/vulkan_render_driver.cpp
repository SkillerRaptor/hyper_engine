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
#include "drivers/vulkan/vulkan_render_conversion.hpp"

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
        .usage = map_buffer_usage_flags(usage),
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
        .magFilter = map_filter(mag_filter),
        .minFilter = map_filter(min_filter),
        .mipmapMode = map_sampler_mipmap_mode(mipmap_filter),
        .addressModeU = map_sampler_address_mode(address_mode_u),
        .addressModeV = map_sampler_address_mode(address_mode_v),
        .addressModeW = map_sampler_address_mode(address_mode_w),
        .mipLodBias = mip_lod_bias,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 0.0,
        .compareEnable = VK_TRUE,
        .compareOp = map_compare_operation(compare_operation),
        .minLod = min_lod,
        .maxLod = max_lod,
        .borderColor = map_border_color(border_color),
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
    VkImageCreateFlags image_create_flags = 0;
    if (dimension == Dimension::TextureCube)
    {
        image_create_flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    const VkImageCreateInfo image_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = image_create_flags,
        .imageType = map_image_type(dimension),
        .format = map_format(format),
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
        .usage = map_image_usage_flags(usage, format),
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

    const VkImageView image_view = create_internal_image_view(image, dimension, format, mip_levels, array_size);

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

PipelineLayout *VulkanRenderDriver::create_pipeline_layout(const std::optional<std::string> &label, const size_t push_constant_size) const
{
    const VkPushConstantRange push_constant_range = {
        .stageFlags = VK_SHADER_STAGE_ALL,
        .offset = 0,
        .size = static_cast<uint32_t>(push_constant_size),
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
        .topology = map_primitive_topology(primitive_state.topology),
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

    const VkPolygonMode polygon_mode = map_polygon_mode(primitive_state.polygon_mode);
    const VkCullModeFlags cull_mode = map_cull_mode_flags(primitive_state.cull_mode);
    const VkFrontFace front_face = map_front_face(primitive_state.front_face);
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
    const VkCompareOp depth_compare_operation = map_compare_operation(depth_stencil_state.depth_compare_operation);
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

        const VkBlendFactor src_color_blend_factor = map_blend_factor(blend_state.src_blend_factor);
        const VkBlendFactor dst_color_blend_factor = map_blend_factor(blend_state.dst_blend_factor);
        const VkBlendOp color_blend_operation = map_blend_operation(blend_state.operation);
        const VkBlendFactor alpha_src_blend_factor = map_blend_factor(blend_state.alpha_src_blend_factor);
        const VkBlendFactor alpha_dst_blend_factor = map_blend_factor(blend_state.alpha_dst_blend_factor);
        const VkBlendOp alpha_blend_operation = map_blend_operation(blend_state.alpha_operation);
        const VkColorComponentFlags color_write_mask = map_color_component_flags(blend_state.color_writes);
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
        color_attachment_formats.push_back(map_format(color_attachment_state.format));
    }

    const VkPipelineRenderingCreateInfo rendering_create_info = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = static_cast<uint32_t>(color_attachment_formats.size()),
        .pColorAttachmentFormats = color_attachment_formats.data(),
        .depthAttachmentFormat = map_format(depth_stencil_state.depth_format),
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
        transition_texture_layout(command_buffer, swapchain_texture, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR, 0, 0);
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
    const uint64_t src_offset,
    const Buffer *dst,
    const uint64_t dst_offset,
    const size_t size) const
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
    const uint64_t src_offset,
    Texture *dst,
    const Offset3d dst_offset,
    const Extent3d dst_extent,
    const uint32_t dst_mip_level,
    const uint32_t dst_array_index) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanBuffer *vulkan_src = reinterpret_cast<const VulkanBuffer *>(src);
    const VulkanTexture *vulkan_dst = reinterpret_cast<const VulkanTexture *>(dst);

    transition_texture_layout(command_buffer, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dst_mip_level, dst_array_index);

    const VkImageSubresourceLayers subresource_layers = {
        .aspectMask = map_image_aspect_flags(vulkan_dst->format),
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
    const uint64_t dst_offset) const
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanTexture *vulkan_src = reinterpret_cast<const VulkanTexture *>(src);
    const VulkanBuffer *vulkan_dst = reinterpret_cast<const VulkanBuffer *>(dst);

    transition_texture_layout(command_buffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, src_mip_level, src_array_index);

    const VkImageSubresourceLayers subresource_layers = {
        .aspectMask = map_image_aspect_flags(vulkan_src->format),
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

    transition_texture_layout(command_buffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, src_mip_level, src_array_index);
    transition_texture_layout(command_buffer, dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, dst_mip_level, dst_array_index);

    const VkImageCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2,
        .pNext = nullptr,
        .srcSubresource =
            {
                .aspectMask = map_image_aspect_flags(vulkan_src->format),
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
                .aspectMask = map_image_aspect_flags(vulkan_dst->format),
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
    const size_t size)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    const VulkanPipelineLayout *vulkan_pipeline_layout = reinterpret_cast<const VulkanPipelineLayout *>(pipeline_layout);

    vkCmdPushConstants(
        vulkan_command_buffer->command_buffer,
        vulkan_pipeline_layout->pipeline_layout,
        VK_SHADER_STAGE_ALL,
        0,
        static_cast<uint32_t>(size),
        data);
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

        transition_texture_layout(command_buffer, color_attachment.texture, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, 0, 0);

        const VkRenderingAttachmentInfo color_attachment_info = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = vulkan_texture->image_view,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = map_attachment_load_operation(color_attachment.operations.load_operation),
            .storeOp = map_attachment_store_operation(color_attachment.operations.store_operation),
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
            command_buffer, render_pass_depth_stencil_attachment.texture, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, 0, 0);

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
            .loadOp = map_attachment_load_operation(render_pass_depth_stencil_attachment.depth_operations.load_operation),
            .storeOp = map_attachment_store_operation(render_pass_depth_stencil_attachment.depth_operations.store_operation),
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
        const Format format = map_vk_format(surface_format.format);
        const VkImageView image_view = create_internal_image_view(image, Dimension::Texture2D, format, 1, 1);

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

VkImageView VulkanRenderDriver::create_internal_image_view(
    const VkImage image,
    const Dimension dimension,
    const Format format,
    const uint32_t mip_levels,
    const uint32_t array_size) const
{
    constexpr VkComponentMapping component_mapping = {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };

    // FIXME: Add subresource
    const VkImageSubresourceRange subresource_range = {
        .aspectMask = map_image_aspect_flags(format),
        .baseMipLevel = 0,
        .levelCount = mip_levels,
        .baseArrayLayer = 0,
        .layerCount = array_size,
    };

    const VkImageViewCreateInfo image_view_create_info = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = image,
        .viewType = map_image_view_type(dimension),
        .format = map_format(format),
        .components = component_mapping,
        .subresourceRange = subresource_range,
    };

    VkImageView image_view = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateImageView(m_device, &image_view_create_info, nullptr, &image_view), vkCreateImageView);
    HE_ASSERT(image_view != VK_NULL_HANDLE);

    return image_view;
}

void VulkanRenderDriver::transition_texture_layout(
    const CommandBuffer *command_buffer,
    Texture *texture,
    const VkImageLayout new_layout,
    const uint32_t mip_level,
    const uint32_t array_index)
{
    const VulkanCommandBuffer *vulkan_command_buffer = reinterpret_cast<const VulkanCommandBuffer *>(command_buffer);
    VulkanTexture *vulkan_texture = reinterpret_cast<VulkanTexture *>(texture);

    // FIXME: Add more specific src/dst stage/access masks

    // FIXME
    const VkImageSubresourceRange subresource_range = {
        .aspectMask = map_image_aspect_flags(vulkan_texture->format),
        .baseMipLevel = 0,
        .levelCount = VK_REMAINING_MIP_LEVELS,
        .baseArrayLayer = 0,
        .layerCount = VK_REMAINING_ARRAY_LAYERS,
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
