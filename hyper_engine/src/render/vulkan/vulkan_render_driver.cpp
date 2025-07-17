/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "render/vulkan/vulkan_render_driver.hpp"

#include <filesystem>
#include <ranges>

#define VMA_IMPLEMENTATION
#include <SDL3/SDL_vulkan.h>
#include <tracy/Tracy.hpp>
#include <vk_mem_alloc.h>
#include <vulkan/vk_enum_string_helper.h>

#include "core/assertion.hpp"
#include "core/filesystem.hpp"
#include "core/logger.hpp"
#include "render/vulkan/vulkan_render_conversion.hpp"

#define HE_VK_CHECK(result, fn)                                                    \
    do                                                                             \
    {                                                                              \
        const VkResult _result = (result);                                         \
        if ((_result) != VK_SUCCESS)                                               \
        {                                                                          \
            HE_FATAL("{} failed: {}", HE_STRINGIFY(fn), string_VkResult(_result)); \
            std::abort();                                                          \
        }                                                                          \
    } while (0)

std::unique_ptr<VulkanRenderDriver> VulkanRenderDriver::create(void *native_window, const u32 width, const u32 height)
{
    HE_VK_CHECK(volkInitialize(), volkInitialize);

    const Instance instance = create_instance();
    const PhysicalDevice physical_device = choose_physical_device(instance);
    const Device device = create_device(instance, physical_device);
    const VmaAllocator allocator = create_allocator(instance, device);
    const VkSurfaceKHR surface = create_surface(instance, native_window);
    const Swapchain swapchain = create_swapchain(device, surface, width, height, VK_NULL_HANDLE);
    const Descriptors descriptors = create_descriptors(device);

    return std::unique_ptr<VulkanRenderDriver>(
        new VulkanRenderDriver(instance, device, allocator, surface, swapchain, descriptors));
}

VulkanRenderDriver::~VulkanRenderDriver()
{
    for (const VkDescriptorSetLayout &descriptor_set_layout : m_descriptors.descriptor_set_layouts)
    {
        vkDestroyDescriptorSetLayout(m_device.raw, descriptor_set_layout, nullptr);
    }

    vkDestroyDescriptorPool(m_device.raw, m_descriptors.descriptor_pool, nullptr);

    destroy_swapchain();
    vkDestroySurfaceKHR(m_instance.raw, m_surface, nullptr);

    vmaDestroyAllocator(m_allocator);
    vkDestroyDevice(m_device.raw, nullptr);

    if (m_instance.debug_messenger != VK_NULL_HANDLE)
    {
        vkDestroyDebugUtilsMessengerEXT(m_instance.raw, m_instance.debug_messenger, nullptr);
    }

    vkDestroyInstance(m_instance.raw, nullptr);
}

void VulkanRenderDriver::wait_idle() const { HE_VK_CHECK(vkDeviceWaitIdle(m_device.raw), vkDeviceWaitIdle); }

BufferId VulkanRenderDriver::create_buffer(const BufferDescriptor &desc)
{
    const VkBufferCreateInfo buffer_create_info {
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .size = desc.size,
        .usage = map_buffer_usage(desc.usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
    };

    VmaAllocationCreateFlags allocation_flags { 0 };
    // if (staging)
    {
        allocation_flags |= VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }

    const VmaAllocationCreateInfo allocation_create_info {
        .flags = allocation_flags,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 0,
    };

    VkBuffer raw { VK_NULL_HANDLE };
    VmaAllocation allocation { VK_NULL_HANDLE };
    HE_VK_CHECK(vmaCreateBuffer(m_allocator, &buffer_create_info, &allocation_create_info, &raw, &allocation, nullptr),
        vmaCreateBuffer);
    HE_ASSERT(raw != VK_NULL_HANDLE);
    HE_ASSERT(allocation != VK_NULL_HANDLE);

    return BufferId(new VulkanBuffer {
        .raw = raw,
        .allocation = allocation,
    });
}

void VulkanRenderDriver::destroy_buffer(const BufferId id)
{
    const VulkanBuffer *buffer = id.as<VulkanBuffer>();
    vmaDestroyBuffer(m_allocator, buffer->raw, buffer->allocation);
    delete buffer;
}

void *VulkanRenderDriver::map_buffer(const BufferId id) const
{
    const VulkanBuffer *buffer = id.as<VulkanBuffer>();
    void *ptr { nullptr };
    vmaMapMemory(m_allocator, buffer->allocation, &ptr);
    HE_ASSERT(ptr != nullptr);

    return ptr;
}

void VulkanRenderDriver::unmap_buffer(const BufferId id) const
{
    const VulkanBuffer *buffer = id.as<VulkanBuffer>();
    vmaUnmapMemory(m_allocator, buffer->allocation);
}

void VulkanRenderDriver::bind_buffer_to_slot(const BufferId id, const ResourceHandle handle) const
{
    const VulkanBuffer *buffer = id.as<VulkanBuffer>();

    const VkDescriptorBufferInfo buffer_info {
        .buffer = buffer->raw,
        .offset = 0,
        .range = VK_WHOLE_SIZE,
    };

    const VkWriteDescriptorSet descriptor_write {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptors.descriptor_sets[static_cast<usize>(DescriptorType::StorageBuffer)],
        .dstBinding = 0,
        .dstArrayElement = handle.get(),
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
        .pImageInfo = nullptr,
        .pBufferInfo = &buffer_info,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device.raw, 1, &descriptor_write, 0, nullptr);
}

ShaderId VulkanRenderDriver::create_shader(const ShaderDescriptor &desc)
{
    const std::vector<uint8_t> data = filesystem::read_file(desc.path);
    HE_ASSERT(!data.empty());

    const ShaderCompilationDescriptor shader_compilation_descriptor {
        .entry_name = desc.entry,
        .type = desc.type,
        .data = data,
    };

    // FIXME: Move shader compilation out of render driver
    const std::vector<uint8_t> code = m_compiler.compile(shader_compilation_descriptor);
    HE_ASSERT(!code.empty());

    if (!code.empty())
    {
        const std::string file_name = std::filesystem::path(desc.path).filename().string();
        const std::string_view shader_stage = [&desc]()
        {
            switch (desc.type)
            {
            case ShaderType::Compute: return "compute";
            case ShaderType::Fragment: return "fragment";
            case ShaderType::Vertex: return "vertex";
            default: HE_UNREACHABLE();
            }
        }();

        HE_INFO("Compiled '{}' for {} stage successfully", file_name, shader_stage);
    }

    const VkShaderModuleCreateInfo shader_module_create_info {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = code.size(),
        .pCode = reinterpret_cast<const uint32_t *>(code.data()),
    };

    VkShaderModule raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateShaderModule(m_device.raw, &shader_module_create_info, nullptr, &raw), vkCreateShaderModule);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    return ShaderId(new VulkanShader {
        .raw = raw,
    });
}

void VulkanRenderDriver::destroy_shader(const ShaderId id)
{
    const VulkanShader *shader = id.as<VulkanShader>();
    vkDestroyShaderModule(m_device.raw, shader->raw, nullptr);
    delete shader;
}

SamplerId VulkanRenderDriver::create_sampler(const SamplerDescriptor &desc)
{
    const VkSamplerCreateInfo sampler_create_info {
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .magFilter = map_filter(desc.mag_filter),
        .minFilter = map_filter(desc.min_filter),
        .mipmapMode = map_mipmap_mode(desc.mipmap_filter),
        .addressModeU = map_address_mode(desc.address_mode_u),
        .addressModeV = map_address_mode(desc.address_mode_v),
        .addressModeW = map_address_mode(desc.address_mode_w),
        .mipLodBias = 0.0f,
        .anisotropyEnable = VK_FALSE,
        .maxAnisotropy = 0.0,
        .compareEnable = VK_TRUE,
        .compareOp = map_compare_operation(desc.compare_operation),
        .minLod = desc.min_lod,
        .maxLod = desc.max_lod,
        .borderColor = map_border_color(desc.border_color),
        .unnormalizedCoordinates = VK_FALSE,
    };

    VkSampler raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateSampler(m_device.raw, &sampler_create_info, nullptr, &raw), vkCreateSampler);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    return SamplerId(new VulkanSampler {
        .raw = raw,
    });
}

void VulkanRenderDriver::destroy_sampler(const SamplerId id)
{
    const VulkanSampler *sampler = id.as<VulkanSampler>();
    vkDestroySampler(m_device.raw, sampler->raw, nullptr);
    delete sampler;
}

void VulkanRenderDriver::bind_sampler_to_slot(const SamplerId id, const ResourceHandle handle) const
{
    const VulkanSampler *sampler = id.as<VulkanSampler>();

    const VkDescriptorImageInfo image_info = {
        .sampler = sampler->raw,
        .imageView = VK_NULL_HANDLE,
        .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    const VkWriteDescriptorSet descriptor_write {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptors.descriptor_sets[static_cast<usize>(DescriptorType::Sampler)],
        .dstBinding = 0,
        .dstArrayElement = handle.get(),
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
        .pImageInfo = &image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device.raw, 1, &descriptor_write, 0, nullptr);
}

TextureId VulkanRenderDriver::create_texture(const TextureDescriptor &desc)
{
    VkImageCreateFlags flags { 0 };
    if (desc.dimension == Dimension::D2 && desc.extent.depth % 6 == 0 && desc.extent.width == desc.extent.height)
    {
        flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
    }

    const VkImageCreateInfo image_create_info {
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = flags,
        .imageType = map_dimension(desc.dimension),
        .format = map_format(desc.format),
        .extent = {
            .width = desc.extent.width,
            .height = desc.extent.height,
            .depth = desc.dimension != Dimension::D3 ? 1 : desc.extent.depth,
        },
        .mipLevels = desc.mip_levels,
        .arrayLayers = desc.extent.depth,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = map_texture_usage(desc.usage, desc.format),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    constexpr VmaAllocationCreateInfo allocation_create_info {
        .flags = 0,
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr,
        .priority = 0,
    };

    VkImage raw { VK_NULL_HANDLE };
    VmaAllocation allocation { VK_NULL_HANDLE };
    HE_VK_CHECK(vmaCreateImage(m_allocator, &image_create_info, &allocation_create_info, &raw, &allocation, nullptr),
        vmaCreateImage);
    HE_ASSERT(raw != VK_NULL_HANDLE);
    HE_ASSERT(allocation != VK_NULL_HANDLE);

    return TextureId(new VulkanTexture {
        .raw = raw,
        .allocation = allocation,
        .layout = VK_IMAGE_LAYOUT_UNDEFINED,
    });
}

void VulkanRenderDriver::destroy_texture(const TextureId id)
{
    const VulkanTexture *texture = id.as<VulkanTexture>();
    for (const TextureViewId texture_view : texture->views)
    {
        destroy_texture_view(texture_view);
    }

    if (texture->allocation != VK_NULL_HANDLE)
    {
        vmaDestroyImage(m_allocator, texture->raw, texture->allocation);
    }
    delete texture;
}

void VulkanRenderDriver::generate_mip_maps(const CommandBufferId id, const TextureId texture_id)
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    VulkanTexture *texture = texture_id.as<VulkanTexture>();

    {
        const VkImageSubresourceRange subresource_range {
            .aspectMask = map_aspect(texture->desc.format),
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = texture->desc.extent.depth,
        };

        const VkImageMemoryBarrier2 image_memory_barrier {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .pNext = nullptr,
            .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
            .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
            .oldLayout = texture->layout,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .srcQueueFamilyIndex = 0,
            .dstQueueFamilyIndex = 0,
            .image = texture->raw,
            .subresourceRange = subresource_range,
        };

        const VkDependencyInfo dependency_info {
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

        vkCmdPipelineBarrier2(command_buffer->raw, &dependency_info);
    }

    for (u32 layer { 0 }; layer != texture->desc.extent.depth; ++layer)
    {
        for (u32 mip { 1 }; mip < texture->desc.mip_levels; ++mip)
        {
            const VkImageBlit image_blit {
                .srcSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = mip - 1,
                    .baseArrayLayer = layer,
                    .layerCount = 1,
                },
                .srcOffsets = {
                    {
                        .x = 0,
                        .y = 0,
                        .z = 0,
                    },
                    {
                        .x = static_cast<i32>(texture->desc.extent.width >> (mip - 1)),
                        .y = static_cast<i32>(texture->desc.extent.height >> (mip - 1)),
                        .z = 1,
                    }
                },
                .dstSubresource = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .mipLevel = mip,
                    .baseArrayLayer = layer,
                    .layerCount = 1,
                },
                .dstOffsets = {
                    {
                        .x = 0,
                        .y = 0,
                        .z = 0,
                    },
                    {
                        .x = static_cast<i32>(texture->desc.extent.width >> mip),
                        .y = static_cast<i32>(texture->desc.extent.height >> mip),
                        .z = 1,
                    }
                },
            };

            {
                const VkImageSubresourceRange subresource_range {
                    .aspectMask = map_aspect(texture->desc.format),
                    .baseMipLevel = mip,
                    .levelCount = 1,
                    .baseArrayLayer = layer,
                    .layerCount = 1,
                };

                const VkImageMemoryBarrier2 image_memory_barrier {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                    .pNext = nullptr,
                    .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                    .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                    .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
                    .oldLayout = texture->layout,
                    .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    .srcQueueFamilyIndex = 0,
                    .dstQueueFamilyIndex = 0,
                    .image = texture->raw,
                    .subresourceRange = subresource_range,
                };

                const VkDependencyInfo dependency_info {
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

                vkCmdPipelineBarrier2(command_buffer->raw, &dependency_info);
            }

            vkCmdBlitImage(command_buffer->raw, texture->raw, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, texture->raw,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_blit, VK_FILTER_LINEAR);

            {
                const VkImageSubresourceRange subresource_range {
                    .aspectMask = map_aspect(texture->desc.format),
                    .baseMipLevel = mip,
                    .levelCount = 1,
                    .baseArrayLayer = layer,
                    .layerCount = 1,
                };

                const VkImageMemoryBarrier2 image_memory_barrier {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
                    .pNext = nullptr,
                    .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                    .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT,
                    .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
                    .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
                    .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                    .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    .srcQueueFamilyIndex = 0,
                    .dstQueueFamilyIndex = 0,
                    .image = texture->raw,
                    .subresourceRange = subresource_range,
                };

                const VkDependencyInfo dependency_info {
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

                vkCmdPipelineBarrier2(command_buffer->raw, &dependency_info);
            }
        }
    }

    texture->layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

    transition_texture_layout(command_buffer->raw, texture, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void VulkanRenderDriver::transition_to_general(const CommandBufferId id, const TextureId texture_id)
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    VulkanTexture *texture = texture_id.as<VulkanTexture>();

    transition_texture_layout(command_buffer->raw, texture, VK_IMAGE_LAYOUT_GENERAL);
}

TextureViewId VulkanRenderDriver::create_texture_view(const TextureViewDescriptor &desc)
{
    const VulkanTexture *texture = desc.texture.as<VulkanTexture>();

    constexpr VkComponentMapping component_mapping {
        .r = VK_COMPONENT_SWIZZLE_IDENTITY,
        .g = VK_COMPONENT_SWIZZLE_IDENTITY,
        .b = VK_COMPONENT_SWIZZLE_IDENTITY,
        .a = VK_COMPONENT_SWIZZLE_IDENTITY,
    };

    const ViewDimension view_dimension = [&desc, &texture]()
    {
        if (desc.dimension != ViewDimension::Unknown)
        {
            return desc.dimension;
        }

        switch (texture->desc.dimension)
        {
        case Dimension::D1: return ViewDimension::D1;
        case Dimension::D2: return texture->desc.extent.depth == 1 ? ViewDimension::D2 : ViewDimension::D2Array;
        case Dimension::D3: return ViewDimension::D3;
        case Dimension::Unknown:;
        default: HE_UNREACHABLE();
        }
    }();

    const Format format = texture->desc.format;
    /*
    const uint32_t mip_levels = desc.mip_levels.value_or(texture.desc.mip_levels);
    const uint32_t array_size = [&desc, &texture, &view_dimension]()
    {
        if (desc.array_layers.has_value())
        {
            return desc.array_layers.value();
        }

        switch (view_dimension)
        {
        case ViewDimension::D1:;
        case ViewDimension::D2:;
        case ViewDimension::D3: return 1;
        case ViewDimension::Cube: return 6;
        case ViewDimension::D2Array:;
        case ViewDimension::CubeArray: return texture.desc.extent.depth;
        case ViewDimension::Unknown:;
        default: HE_UNREACHABLE();
        }

        return 0;
    }();
    */

    const VkImageSubresourceRange subresource_range {
        .aspectMask = map_aspect(format),
        .baseMipLevel = desc.base_mip_level,
        .levelCount = desc.mip_levels,
        .baseArrayLayer = desc.base_array_layer,
        .layerCount = desc.array_layers,
    };

    const VkImageViewCreateInfo image_view_create_info {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = texture->raw,
        .viewType = map_view_dimension(view_dimension),
        .format = map_format(format),
        .components = component_mapping,
        .subresourceRange = subresource_range,
    };

    VkImageView raw = VK_NULL_HANDLE;
    HE_VK_CHECK(vkCreateImageView(m_device.raw, &image_view_create_info, nullptr, &raw), vkCreateImageView);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    return TextureViewId(new VulkanTextureView {
        .raw = raw,
    });
}

void VulkanRenderDriver::destroy_texture_view(const TextureViewId id)
{
    const VulkanTextureView *texture_view = id.as<VulkanTextureView>();
    vkDestroyImageView(m_device.raw, texture_view->raw, nullptr);
    delete texture_view;
}

void VulkanRenderDriver::bind_texture_view_to_slot(const TextureViewId id, const ResourceHandle handle) const
{
    const VulkanTextureView *texture_view = id.as<VulkanTextureView>();
    const VulkanTexture *texture = texture_view->desc.texture.as<VulkanTexture>();

    const VkDescriptorImageInfo image_info = {
        .sampler = VK_NULL_HANDLE,
        .imageView = texture_view->raw,
        .imageLayout = VK_IMAGE_LAYOUT_GENERAL,
    };

    const VkWriteDescriptorSet sampled_descriptor_write = {
        .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
        .pNext = nullptr,
        .dstSet = m_descriptors.descriptor_sets[static_cast<usize>(DescriptorType::SampledImage)],
        .dstBinding = 0,
        .dstArrayElement = handle.get(),
        .descriptorCount = 1,
        .descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
        .pImageInfo = &image_info,
        .pBufferInfo = nullptr,
        .pTexelBufferView = nullptr,
    };

    vkUpdateDescriptorSets(m_device.raw, 1, &sampled_descriptor_write, 0, nullptr);

    if ((texture->desc.usage & TextureUsage::Storage) == TextureUsage::Storage)
    {
        const VkWriteDescriptorSet storage_descriptor_write = {
            .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
            .pNext = nullptr,
            .dstSet = m_descriptors.descriptor_sets[static_cast<usize>(DescriptorType::StorageImage)],
            .dstBinding = 0,
            .dstArrayElement = handle.get(),
            .descriptorCount = 1,
            .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
            .pImageInfo = &image_info,
            .pBufferInfo = nullptr,
            .pTexelBufferView = nullptr,
        };

        vkUpdateDescriptorSets(m_device.raw, 1, &storage_descriptor_write, 0, nullptr);
    }
}

PipelineLayoutId VulkanRenderDriver::create_pipeline_layout(const PipelineLayoutDescriptor &desc)
{
    const VkPushConstantRange push_constant_range {
        .stageFlags = VK_SHADER_STAGE_ALL,
        .offset = 0,
        .size = desc.push_constant_size,
    };

    const VkPipelineLayoutCreateInfo pipeline_layout_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = static_cast<u32>(m_descriptors.descriptor_set_layouts.size()),
        .pSetLayouts = m_descriptors.descriptor_set_layouts.data(),
        .pushConstantRangeCount = static_cast<u32>(desc.push_constant_size == 0 ? 0 : 1),
        .pPushConstantRanges = desc.push_constant_size == 0 ? nullptr : &push_constant_range,
    };

    VkPipelineLayout raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreatePipelineLayout(m_device.raw, &pipeline_layout_create_info, nullptr, &raw), vkCreatePipelineLayout);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    return PipelineLayoutId(new VulkanPipelineLayout {
        .raw = raw,
    });
}

void VulkanRenderDriver::destroy_pipeline_layout(const PipelineLayoutId id)
{
    const VulkanPipelineLayout *pipeline_layout = id.as<VulkanPipelineLayout>();
    vkDestroyPipelineLayout(m_device.raw, pipeline_layout->raw, nullptr);
    delete pipeline_layout;
}

ComputePipelineId VulkanRenderDriver::create_compute_pipeline(const ComputePipelineDescriptor &desc)
{
    const VulkanShader *shader = desc.shader.as<VulkanShader>();
    const VkPipelineShaderStageCreateInfo pipeline_shader_stage_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = shader->raw,
        .pName = shader->desc.entry.c_str(),
        .pSpecializationInfo = nullptr,
    };

    const VulkanPipelineLayout *pipeline_layout = desc.layout.as<VulkanPipelineLayout>();
    const VkComputePipelineCreateInfo compute_pipeline_create_info {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = pipeline_shader_stage_create_info,
        .layout = pipeline_layout->raw,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipeline raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateComputePipelines(m_device.raw, VK_NULL_HANDLE, 1, &compute_pipeline_create_info, nullptr, &raw),
        vkCreateComputePipelines);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    return ComputePipelineId(new VulkanComputePipeline {
        .raw = raw,
    });
}

void VulkanRenderDriver::destroy_compute_pipeline(const ComputePipelineId id)
{
    const VulkanComputePipeline *compute_pipeline = id.as<VulkanComputePipeline>();
    vkDestroyPipeline(m_device.raw, compute_pipeline->raw, nullptr);
    delete compute_pipeline;
}

RenderPipelineId VulkanRenderDriver::create_render_pipeline(const RenderPipelineDescriptor &desc)
{
    const VulkanShader *vertex_shader = desc.vertex_shader.as<VulkanShader>();
    const VkPipelineShaderStageCreateInfo vertex_pipeline_shader_stage_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertex_shader->raw,
        .pName = vertex_shader->desc.entry.c_str(),
        .pSpecializationInfo = nullptr,
    };

    const VulkanShader *fragment_shader = desc.fragment_shader.as<VulkanShader>();
    const VkPipelineShaderStageCreateInfo fragment_pipeline_shader_stage_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragment_shader->raw,
        .pName = fragment_shader->desc.entry.c_str(),
        .pSpecializationInfo = nullptr,
    };

    const std::array<VkPipelineShaderStageCreateInfo, 2> shader_stage_create_infos {
        vertex_pipeline_shader_stage_create_info,
        fragment_pipeline_shader_stage_create_info,
    };

    constexpr VkPipelineVertexInputStateCreateInfo vertex_input_state_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };

    const VkPipelineInputAssemblyStateCreateInfo input_assembly_state_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = map_primitive_topology(desc.primitive_state.topology),
        .primitiveRestartEnable = false,
    };

    constexpr VkPipelineTessellationStateCreateInfo tessellation_state_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .patchControlPoints = 0,
    };

    constexpr VkPipelineViewportStateCreateInfo viewport_state_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = nullptr,
        .scissorCount = 1,
        .pScissors = nullptr,
    };

    VkPipelineRasterizationStateCreateInfo rasterization_state_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = false,
        .rasterizerDiscardEnable = false,
        .polygonMode = map_polygon_mode(desc.primitive_state.polygon_mode),
        .cullMode = map_cull_mode(desc.primitive_state.cull_mode),
        .frontFace = map_front_face(desc.primitive_state.front_face),
        .depthBiasEnable = false,
        .depthBiasConstantFactor = 0.0f,
        .depthBiasClamp = 0.0f,
        .depthBiasSlopeFactor = 0.0f,
        .lineWidth = 1.0,
    };

    if (desc.depth_stencil_state.has_value())
    {
        const DepthStencilState &depth_stencil_state = desc.depth_stencil_state.value();
        rasterization_state_create_info.depthBiasEnable = depth_stencil_state.depth_bias_state.enable;
        rasterization_state_create_info.depthBiasConstantFactor = depth_stencil_state.depth_bias_state.constant;
        rasterization_state_create_info.depthBiasClamp = depth_stencil_state.depth_bias_state.clamp;
        rasterization_state_create_info.depthBiasSlopeFactor = depth_stencil_state.depth_bias_state.slope;
    }

    // FIXME: Add multisampling
    constexpr VkPipelineMultisampleStateCreateInfo multisample_state_create_info {
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
    VkPipelineDepthStencilStateCreateInfo depth_stencil_state_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthTestEnable = false,
        .depthWriteEnable = false,
        .depthCompareOp = VK_COMPARE_OP_NEVER,
        .depthBoundsTestEnable = false,
        .stencilTestEnable = false,
        .front = {},
        .back = {},
        .minDepthBounds = 0.0,
        .maxDepthBounds = 1.0,
    };

    if (desc.depth_stencil_state.has_value())
    {
        const DepthStencilState &depth_stencil_state = desc.depth_stencil_state.value();
        depth_stencil_state_create_info.depthTestEnable = depth_stencil_state.depth_test_enable;
        depth_stencil_state_create_info.depthWriteEnable = depth_stencil_state.depth_write_enable;

        const VkCompareOp depth_compare_operation = map_compare_operation(depth_stencil_state.depth_compare_operation);
        depth_stencil_state_create_info.depthCompareOp = depth_compare_operation;
    }

    std::vector<VkPipelineColorBlendAttachmentState> color_blend_attachment_states {};
    for (const ColorAttachmentState &color_attachment_state : desc.color_attachment_states)
    {
        const BlendState &blend_state = color_attachment_state.blend_state;

        const VkPipelineColorBlendAttachmentState color_blend_attachment_state {
            .blendEnable = blend_state.enable,
            .srcColorBlendFactor = map_blend_factor(blend_state.src_factor),
            .dstColorBlendFactor = map_blend_factor(blend_state.dst_factor),
            .colorBlendOp = map_blend_operation(blend_state.operation),
            .srcAlphaBlendFactor = map_blend_factor(blend_state.alpha_src_factor),
            .dstAlphaBlendFactor = map_blend_factor(blend_state.alpha_dst_factor),
            .alphaBlendOp = map_blend_operation(blend_state.alpha_operation),
            .colorWriteMask = map_color_writes(blend_state.color_writes),
        };

        color_blend_attachment_states.push_back(color_blend_attachment_state);
    }

    const VkPipelineColorBlendStateCreateInfo color_blend_state_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = false,
        .logicOp = VK_LOGIC_OP_NO_OP,
        .attachmentCount = static_cast<u32>(color_blend_attachment_states.size()),
        .pAttachments = color_blend_attachment_states.data(),
        .blendConstants = { 0.0, 0.0, 0.0, 0.0 },
    };

    constexpr std::array<VkDynamicState, 2> dynamic_states {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };

    const VkPipelineDynamicStateCreateInfo dynamic_state_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = static_cast<u32>(dynamic_states.size()),
        .pDynamicStates = dynamic_states.data(),
    };

    std::vector<VkFormat> color_attachment_formats {};
    for (const ColorAttachmentState &color_attachment_state : desc.color_attachment_states)
    {
        color_attachment_formats.push_back(map_format(color_attachment_state.format));
    }

    const VkFormat depth_attachment_format = desc.depth_stencil_state.has_value()
        ? map_format(desc.depth_stencil_state.value().depth_format)
        : VK_FORMAT_UNDEFINED;
    const VkPipelineRenderingCreateInfo rendering_create_info {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
        .pNext = nullptr,
        .viewMask = 0,
        .colorAttachmentCount = static_cast<u32>(color_attachment_formats.size()),
        .pColorAttachmentFormats = color_attachment_formats.data(),
        .depthAttachmentFormat = depth_attachment_format,
        .stencilAttachmentFormat = VK_FORMAT_UNDEFINED,
    };

    const VulkanPipelineLayout *layout = desc.layout.as<VulkanPipelineLayout>();
    const VkGraphicsPipelineCreateInfo graphics_pipeline_create_info {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = &rendering_create_info,
        .flags = 0,
        .stageCount = static_cast<u32>(shader_stage_create_infos.size()),
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
        .layout = layout->raw,
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    VkPipeline raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateGraphicsPipelines(m_device.raw, VK_NULL_HANDLE, 1, &graphics_pipeline_create_info, nullptr, &raw),
        vkCreateGraphicsPipelines);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    return RenderPipelineId(new VulkanRenderPipeline {
        .raw = raw,
    });
}

void VulkanRenderDriver::destroy_render_pipeline(const RenderPipelineId id)
{
    const VulkanRenderPipeline *render_pipeline = id.as<VulkanRenderPipeline>();
    vkDestroyPipeline(m_device.raw, render_pipeline->raw, nullptr);
    delete render_pipeline;
}

CommandBufferId VulkanRenderDriver::create_command_buffer()
{
    const VkCommandPoolCreateInfo command_pool_create_info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = m_device.queue_family,
    };

    VkCommandPool command_pool { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateCommandPool(m_device.raw, &command_pool_create_info, nullptr, &command_pool), vkCreateCommandPool);
    HE_ASSERT(command_pool != VK_NULL_HANDLE);

    const VkCommandBufferAllocateInfo command_buffer_allocate_info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
        .pNext = nullptr,
        .commandPool = command_pool,
        .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .commandBufferCount = 1,
    };

    VkCommandBuffer raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vkAllocateCommandBuffers(m_device.raw, &command_buffer_allocate_info, &raw), vkAllocateCommandBuffers);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    constexpr VkFenceCreateInfo fence_create_info {
        .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
    };

    VkFence fence { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateFence(m_device.raw, &fence_create_info, nullptr, &fence), vkCreateFence);
    HE_ASSERT(fence != VK_NULL_HANDLE);

    VkSemaphoreTypeCreateInfo submit_semaphore_type_create_info {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO,
        .pNext = nullptr,
        .semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE,
        .initialValue = 0,
    };

    const VkSemaphoreCreateInfo submit_semaphore_create_info {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        .pNext = &submit_semaphore_type_create_info,
        .flags = 0,
    };

    VkSemaphore semaphore { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateSemaphore(m_device.raw, &submit_semaphore_create_info, nullptr, &semaphore), vkCreateSemaphore);
    HE_ASSERT(semaphore != VK_NULL_HANDLE);

    return CommandBufferId(new VulkanCommandBuffer {
        .command_pool = command_pool,
        .raw = raw,
        .render_fence = fence,
        .submit_semaphore = semaphore,
        .semaphore_counter = 0,
        .swapchain_texture_acquired = false,
    });
}

void VulkanRenderDriver::destroy_command_buffer(const CommandBufferId id)
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkDestroyCommandPool(m_device.raw, command_buffer->command_pool, nullptr);
    vkDestroyFence(m_device.raw, command_buffer->render_fence, nullptr);
    vkDestroySemaphore(m_device.raw, command_buffer->submit_semaphore, nullptr);
    delete command_buffer;
}

void VulkanRenderDriver::acquire_command_buffer(const CommandBufferId id)
{
    ZoneScopedNC("AcquireCommandBuffer", 0xff8a02);

    VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    command_buffer->swapchain_texture_acquired = false;

    const uint64_t wait_frame_index = command_buffer->semaphore_counter;
    const VkSemaphoreWaitInfo semaphore_wait_info {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
        .pNext = nullptr,
        .flags = 0,
        .semaphoreCount = 1,
        .pSemaphores = &command_buffer->submit_semaphore,
        .pValues = &wait_frame_index,
    };
    HE_VK_CHECK(
        vkWaitSemaphores(m_device.raw, &semaphore_wait_info, std::numeric_limits<uint64_t>::max()), vkWaitSemaphores);

    HE_VK_CHECK(vkResetCommandBuffer(command_buffer->raw, 0), vkResetCommandBuffer);

    constexpr VkCommandBufferBeginInfo command_buffer_begin_info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        .pInheritanceInfo = nullptr,
    };

    HE_VK_CHECK(vkBeginCommandBuffer(command_buffer->raw, &command_buffer_begin_info), vkBeginCommandBuffer);
}

void VulkanRenderDriver::submit_command_buffer(const CommandBufferId id)
{
    ZoneScopedNC("SubmitCommandBuffer", 0xff8a02);

    VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    if (command_buffer->swapchain_texture_acquired)
    {
        VulkanTexture *swapchain_texture = m_swapchain.textures[m_swapchain.texture_index].as<VulkanTexture>();
        transition_texture_layout(command_buffer->raw, swapchain_texture, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
    }

    HE_VK_CHECK(vkEndCommandBuffer(command_buffer->raw), vkEndCommandBuffer);

    if (command_buffer->swapchain_texture_acquired)
    {
        HE_VK_CHECK(
            vkWaitForFences(m_device.raw, 1, &command_buffer->render_fence, VK_TRUE, std::numeric_limits<uint64_t>::max()),
            vkWaitForFences);
        HE_VK_CHECK(vkResetFences(m_device.raw, 1, &command_buffer->render_fence), vkResetFences);
    }

    command_buffer->semaphore_counter += 1;

    const VkCommandBufferSubmitInfo command_buffer_submit_info {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
        .pNext = nullptr,
        .commandBuffer = command_buffer->raw,
        .deviceMask = 0,
    };

    const VkSemaphoreSubmitInfo submit_semaphore_submit_info {
        .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
        .pNext = nullptr,
        .semaphore = command_buffer->submit_semaphore,
        .value = command_buffer->semaphore_counter,
        .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        .deviceIndex = 0,
    };

    const VkSubmitInfo2 submit_info {
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

    HE_VK_CHECK(vkQueueSubmit2(m_device.queue, 1, &submit_info, VK_NULL_HANDLE), vkQueueSubmit2);

    if (command_buffer->swapchain_texture_acquired)
    {
        present();
    }
}

void VulkanRenderDriver::begin_gpu_marker(const CommandBufferId id, const PassLabel label) const
{
    if (m_instance.debug_messenger == VK_NULL_HANDLE)
    {
        return;
    }

    const f32 r = static_cast<f32>(label.color.r) / 255.0f;
    const f32 g = static_cast<f32>(label.color.g) / 255.0f;
    const f32 b = static_cast<f32>(label.color.b) / 255.0f;
    const VkDebugUtilsLabelEXT label_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        .pNext = nullptr,
        .pLabelName = label.name.data(),
        .color = { r, g, b, 1.0f },
    };

    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkCmdBeginDebugUtilsLabelEXT(command_buffer->raw, &label_info);
}

void VulkanRenderDriver::end_gpu_marker(const CommandBufferId id) const
{
    if (m_instance.debug_messenger == VK_NULL_HANDLE)
    {
        return;
    }

    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkCmdEndDebugUtilsLabelEXT(command_buffer->raw);
}

void VulkanRenderDriver::insert_gpu_marker(const CommandBufferId id, const PassLabel label) const
{
    if (m_instance.debug_messenger == VK_NULL_HANDLE)
    {
        return;
    }

    const f32 r = static_cast<f32>(label.color.r) / 255.0f;
    const f32 g = static_cast<f32>(label.color.g) / 255.0f;
    const f32 b = static_cast<f32>(label.color.b) / 255.0f;
    const VkDebugUtilsLabelEXT label_info = {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT,
        .pNext = nullptr,
        .pLabelName = label.name.data(),
        .color = { r, g, b, 1.0f },
    };

    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkCmdInsertDebugUtilsLabelEXT(command_buffer->raw, &label_info);
}

std::pair<TextureViewId, bool> VulkanRenderDriver::acquire_swapchain_texture(const CommandBufferId id)
{
    ZoneScopedNC("AcquireSwapchainTexture", 0xff8a02);

    bool recreated = false;
    if (m_swapchain.out_of_date)
    {
        recreate_swapchain();
        recreated = true;
    }

    VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    while (true)
    {
        const VkResult result = vkAcquireNextImageKHR(m_device.raw, m_swapchain.raw, std::numeric_limits<uint64_t>::max(),
            VK_NULL_HANDLE, command_buffer->render_fence, &m_swapchain.texture_index);

        if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
        {
            break;
        }

        recreate_swapchain();
        recreated = true;
    }

    command_buffer->swapchain_texture_acquired = true;

    return {
        m_swapchain.textures[m_swapchain.texture_index].as<VulkanTexture>()->views[0],
        recreated,
    };
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
        .pSwapchains = &m_swapchain.raw,
        .pImageIndices = &m_swapchain.texture_index,
        .pResults = nullptr,
    };

    const VkResult result = vkQueuePresentKHR(m_device.queue, &present_info);
    if (result == VK_SUBOPTIMAL_KHR || result == VK_ERROR_OUT_OF_DATE_KHR)
    {
        m_swapchain.out_of_date = true;
    }
    else
    {
        HE_VK_CHECK(result, vkQueuePresentKHR);
    }
}

std::vector<TextureViewId> VulkanRenderDriver::query_swapchain_texture_views() const
{
    const std::vector<TextureViewId> texture_views = m_swapchain.textures
        | std::views::transform(
            [](const TextureId texture_id)
            {
                const VulkanTexture *texture = texture_id.as<VulkanTexture>();
                return texture->views[0];
            })
        | std::ranges::to<std::vector<TextureViewId>>();

    return texture_views;
}

// FIXME: Check if the window id matches and maybe save the width/height
void VulkanRenderDriver::on_resize(const WindowResizeEvent &event)
{
    m_swapchain.width = event.width();
    m_swapchain.height = event.height();
    m_swapchain.out_of_date = true;
}

void VulkanRenderDriver::copy_buffer_to_buffer(
    const CommandBufferId id, const BufferTargetDescriptor src, const BufferTargetDescriptor dst, const usize size) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    const VulkanBuffer *vulkan_src = src.buffer.as<VulkanBuffer>();
    const VulkanBuffer *vulkan_dst = dst.buffer.as<VulkanBuffer>();

    const VkBufferCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_COPY_2,
        .pNext = nullptr,
        .srcOffset = src.offset,
        .dstOffset = dst.offset,
        .size = size,
    };

    const VkCopyBufferInfo2 copy_buffer_info = {
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2,
        .pNext = nullptr,
        .srcBuffer = vulkan_src->raw,
        .dstBuffer = vulkan_dst->raw,
        .regionCount = 1,
        .pRegions = &region,
    };

    vkCmdCopyBuffer2(command_buffer->raw, &copy_buffer_info);
}

void VulkanRenderDriver::copy_buffer_to_texture(const CommandBufferId id,
    const BufferTargetDescriptor src,
    const TextureTargetDescriptor dst,
    const Extent3d extent) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    const VulkanBuffer *vulkan_src = src.buffer.as<VulkanBuffer>();
    VulkanTexture *vulkan_dst = dst.texture.as<VulkanTexture>();

    transition_texture_layout(command_buffer->raw, vulkan_dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    const VkImageSubresourceLayers subresource_layers = {
        .aspectMask = map_aspect(vulkan_dst->desc.format),
        .mipLevel = 0,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const VkBufferImageCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
        .pNext = nullptr,
        .bufferOffset = src.offset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = subresource_layers,
        .imageOffset = map_offset_3d(dst.offset),
        .imageExtent = map_extent_3d(extent),
    };

    const VkCopyBufferToImageInfo2 copy_buffer_to_image_info = {
        .sType = VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2,
        .pNext = nullptr,
        .srcBuffer = vulkan_src->raw,
        .dstImage = vulkan_dst->raw,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount = 1,
        .pRegions = &region,
    };

    vkCmdCopyBufferToImage2(command_buffer->raw, &copy_buffer_to_image_info);
}

void VulkanRenderDriver::copy_texture_to_buffer(const CommandBufferId id,
    const TextureTargetDescriptor src,
    const BufferTargetDescriptor dst,
    const Extent3d extent) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    VulkanTexture *vulkan_src = src.texture.as<VulkanTexture>();
    const VulkanBuffer *vulkan_dst = dst.buffer.as<VulkanBuffer>();

    transition_texture_layout(command_buffer->raw, vulkan_src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    const VkImageSubresourceLayers subresource_layers = {
        .aspectMask = map_aspect(vulkan_src->desc.format),
        .mipLevel = 0,
        .baseArrayLayer = 0,
        .layerCount = 1,
    };

    const VkBufferImageCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2,
        .pNext = nullptr,
        .bufferOffset = dst.offset,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = subresource_layers,
        .imageOffset = map_offset_3d(src.offset),
        .imageExtent = map_extent_3d(extent),
    };

    const VkCopyImageToBufferInfo2 copy_image_to_buffer_info = {
        .sType = VK_STRUCTURE_TYPE_COPY_IMAGE_TO_BUFFER_INFO_2,
        .pNext = nullptr,
        .srcImage = vulkan_src->raw,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstBuffer = vulkan_dst->raw,
        .regionCount = 1,
        .pRegions = &region,
    };

    vkCmdCopyImageToBuffer2(command_buffer->raw, &copy_image_to_buffer_info);
}

void VulkanRenderDriver::copy_texture_to_texture(const CommandBufferId id,
    const TextureTargetDescriptor src,
    const TextureTargetDescriptor dst,
    const Extent3d extent) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    VulkanTexture *vulkan_src = src.texture.as<VulkanTexture>();
    VulkanTexture *vulkan_dst = dst.texture.as<VulkanTexture>();

    transition_texture_layout(command_buffer->raw, vulkan_src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
    transition_texture_layout(command_buffer->raw, vulkan_dst, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    const VkImageCopy2 region = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_COPY_2,
        .pNext = nullptr,
        .srcSubresource =
            {
                .aspectMask = map_aspect(vulkan_src->desc.format),
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        .srcOffset =map_offset_3d(src.offset),
        .dstSubresource =
            {
                .aspectMask = map_aspect(vulkan_dst->desc.format),
                .mipLevel = 0,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        .dstOffset = map_offset_3d(dst.offset),
        .extent = map_extent_3d(extent),
    };

    const VkCopyImageInfo2 copy_image_info = {
        .sType = VK_STRUCTURE_TYPE_COPY_IMAGE_INFO_2,
        .pNext = nullptr,
        .srcImage = vulkan_src->raw,
        .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
        .dstImage = vulkan_dst->raw,
        .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        .regionCount = 1,
        .pRegions = &region,
    };

    vkCmdCopyImage2(command_buffer->raw, &copy_image_info);
}

void VulkanRenderDriver::push_constants(
    const CommandBufferId id, const PipelineLayoutId pipeline_layout_id, const void *data, const usize size)
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    const VulkanPipelineLayout *pipeline_layout = pipeline_layout_id.as<VulkanPipelineLayout>();

    vkCmdPushConstants(command_buffer->raw, pipeline_layout->raw, VK_SHADER_STAGE_ALL, 0, static_cast<uint32_t>(size), data);
}

void VulkanRenderDriver::begin_compute_pass(const CommandBufferId, const ComputePassDescriptor &) const
{
    // NOTE: noop
}

void VulkanRenderDriver::end_compute_pass(const CommandBufferId) const
{
    // NOTE: noop
}

void VulkanRenderDriver::bind_compute_pipeline(const CommandBufferId id, const ComputePipelineId compute_pipeline_id) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    const VulkanComputePipeline *compute_pipeline = compute_pipeline_id.as<VulkanComputePipeline>();
    const VulkanPipelineLayout *pipeline_layout = compute_pipeline->desc.layout.as<VulkanPipelineLayout>();

    vkCmdBindDescriptorSets(command_buffer->raw, VK_PIPELINE_BIND_POINT_COMPUTE, pipeline_layout->raw, 0,
        static_cast<uint32_t>(m_descriptors.descriptor_sets.size()), m_descriptors.descriptor_sets.data(), 0, nullptr);
    vkCmdBindPipeline(command_buffer->raw, VK_PIPELINE_BIND_POINT_COMPUTE, compute_pipeline->raw);
}

void VulkanRenderDriver::dispatch(const CommandBufferId id, const u32 x, const u32 y, const u32 z) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkCmdDispatch(command_buffer->raw, x, y, z);
}

void VulkanRenderDriver::begin_render_pass(const CommandBufferId id, const RenderPassDescriptor &desc) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();

    const VulkanTextureView *texture_view = desc.color_attachments[0].view.as<VulkanTextureView>();
    const VulkanTexture *texture = texture_view->desc.texture.as<VulkanTexture>();
    const VkExtent2D extent {
        .width = texture->desc.extent.width,
        .height = texture->desc.extent.height,
    };

    const VkRect2D render_area {
        .offset =
            {
                .x = 0,
                .y = 0,
            },
        .extent = extent,
    };

    constexpr VkClearValue clear_value {
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

    std::vector<VkRenderingAttachmentInfo> rendering_color_attachments {};
    for (const ColorAttachment &color_attachment : desc.color_attachments)
    {
        const VulkanTextureView *color_attachment_texture_view = color_attachment.view.as<VulkanTextureView>();
        VulkanTexture *color_attachment_texture = color_attachment_texture_view->desc.texture.as<VulkanTexture>();

        transition_texture_layout(command_buffer->raw, color_attachment_texture, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

        const VkRenderingAttachmentInfo color_attachment_info {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = color_attachment_texture_view->raw,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = map_load_operation(color_attachment.operations.load_op),
            .storeOp = map_store_operation(color_attachment.operations.store_op),
            .clearValue = clear_value,
        };

        rendering_color_attachments.push_back(color_attachment_info);
    }

    std::optional<VkRenderingAttachmentInfo> depth_attachment_info { std::nullopt };
    if (desc.depth_stencil_attachment.has_value())
    {
        const DepthStencilAttachment &depth_stencil_attachment = desc.depth_stencil_attachment.value();
        const VulkanTextureView *depth_stencil_texture_view = depth_stencil_attachment.view.as<VulkanTextureView>();
        VulkanTexture *depth_stencil_texture = depth_stencil_texture_view->desc.texture.as<VulkanTexture>();

        transition_texture_layout(
            command_buffer->raw, depth_stencil_texture, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        constexpr VkClearValue depth_clear_value {
            .depthStencil =
                {
                    .depth = 1.0f,
                    .stencil = 0,
                },
        };

        depth_attachment_info = {
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .pNext = nullptr,
            .imageView = depth_stencil_texture_view->raw,
            .imageLayout = VK_IMAGE_LAYOUT_ATTACHMENT_OPTIMAL,
            .resolveMode = VK_RESOLVE_MODE_NONE,
            .resolveImageView = VK_NULL_HANDLE,
            .resolveImageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .loadOp = map_load_operation(depth_stencil_attachment.depth_operations.load_op),
            .storeOp = map_store_operation(depth_stencil_attachment.depth_operations.store_op),
            .clearValue = depth_clear_value,
        };
    }

    const VkRenderingInfo rendering_info {
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

    const VkViewport viewport {
        .x = 0.0,
        .y = 0,
        .width = static_cast<float>(extent.width),
        .height = static_cast<float>(extent.height),
        .minDepth = 0.0,
        .maxDepth = 1.0,
    };

    vkCmdBeginRendering(command_buffer->raw, &rendering_info);
    vkCmdSetViewport(command_buffer->raw, 0, 1, &viewport);
    vkCmdSetScissor(command_buffer->raw, 0, 1, &render_area);
}

void VulkanRenderDriver::end_render_pass(const CommandBufferId id) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkCmdEndRendering(command_buffer->raw);
}

void VulkanRenderDriver::bind_render_pipeline(const CommandBufferId id, const RenderPipelineId render_pipeline_id) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    const VulkanRenderPipeline *render_pipeline = render_pipeline_id.as<VulkanRenderPipeline>();
    const VulkanPipelineLayout *pipeline_layout = render_pipeline->desc.layout.as<VulkanPipelineLayout>();

    vkCmdBindDescriptorSets(command_buffer->raw, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline_layout->raw, 0,
        static_cast<uint32_t>(m_descriptors.descriptor_sets.size()), m_descriptors.descriptor_sets.data(), 0, nullptr);
    vkCmdBindPipeline(command_buffer->raw, VK_PIPELINE_BIND_POINT_GRAPHICS, render_pipeline->raw);
}

void VulkanRenderDriver::bind_index_buffer(const CommandBufferId id, const BufferId buffer_id) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    const VulkanBuffer *buffer = buffer_id.as<VulkanBuffer>();

    vkCmdBindIndexBuffer(command_buffer->raw, buffer->raw, 0, VK_INDEX_TYPE_UINT32);
}

void VulkanRenderDriver::set_viewport(const CommandBufferId id,
    const f32 x,
    const f32 y,
    const f32 width,
    const f32 height,
    const f32 min_depth,
    const f32 max_depth) const
{
    const VkViewport viewport = {
        .x = x,
        .y = y,
        .width = width,
        .height = height,
        .minDepth = min_depth,
        .maxDepth = max_depth,
    };

    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkCmdSetViewport(command_buffer->raw, 0, 1, &viewport);
}

void VulkanRenderDriver::set_scissor(const CommandBufferId id, const Offset2d offset, const Extent2d extent) const
{
    const VkRect2D scissor = {
        .offset = map_offset_2d(offset),
        .extent = map_extent_2d(extent),
    };

    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkCmdSetScissor(command_buffer->raw, 0, 1, &scissor);
}

void VulkanRenderDriver::draw(const CommandBufferId id,
    const u32 vertex_count,
    const u32 instance_count,
    const u32 first_vertex,
    const u32 first_instance) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkCmdDraw(command_buffer->raw, vertex_count, instance_count, first_vertex, first_instance);
}

void VulkanRenderDriver::draw_indexed(const CommandBufferId id,
    const u32 index_count,
    const u32 instance_count,
    const u32 first_index,
    const i32 vertex_offset,
    const u32 first_instance) const
{
    const VulkanCommandBuffer *command_buffer = id.as<VulkanCommandBuffer>();
    vkCmdDrawIndexed(command_buffer->raw, index_count, instance_count, first_index, vertex_offset, first_instance);
}

std::vector<std::string> VulkanRenderDriver::find_instance_layers()
{
    u32 layer_count { 0 };
    HE_VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, nullptr), vkEnumerateInstanceLayerProperties);

    std::vector<VkLayerProperties> layers(layer_count);
    HE_VK_CHECK(vkEnumerateInstanceLayerProperties(&layer_count, layers.data()), vkEnumerateInstanceLayerProperties);

    std::unordered_set<std::string_view> requested_layers {};

#if HE_DEBUG_BUILD
    requested_layers.insert(s_validation_layer);
#endif

    const std::vector<std::string> found_layers = layers
        | std::views::filter(
            [&](const VkLayerProperties &properties)
            {
                const std::string_view name = properties.layerName;
                return requested_layers.contains(name);
            })
        | std::views::transform(
            [&](const VkLayerProperties &properties)
            {
                const std::string name = properties.layerName;
                return name;
            })
        | std::ranges::to<std::vector<std::string>>();

    return found_layers;
}

std::unordered_set<std::string> VulkanRenderDriver::find_instance_extensions()
{
    u32 extension_count { 0 };
    HE_VK_CHECK(
        vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr), vkEnumerateInstanceExtensionProperties);

    std::vector<VkExtensionProperties> extensions(extension_count);
    HE_VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extensions.data()),
        vkEnumerateInstanceExtensionProperties);

    std::unordered_set<std::string_view> requested_extensions {};

#if HE_DEBUG_BUILD
    requested_extensions.insert(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    const std::unordered_set<std::string> found_extensions = extensions
        | std::views::filter(
            [&](const VkExtensionProperties &properties)
            {
                const std::string_view name = properties.extensionName;
                return requested_extensions.contains(name);
            })
        | std::views::transform(
            [&](const VkExtensionProperties &properties)
            {
                const std::string name = properties.extensionName;
                return name;
            })
        | std::ranges::to<std::unordered_set<std::string>>();

    return found_extensions;
}

VulkanRenderDriver::Instance VulkanRenderDriver::create_instance()
{
    const std::vector<std::string> layers = find_instance_layers();
    const std::vector<const char *> layers_cstr = layers
        | std::views::transform(
            [](const std::string &value)
            {
                return value.data();
            })
        | std::ranges::to<std::vector<const char *>>();

    const std::unordered_set<std::string> extensions = find_instance_extensions();
    const std::vector<const char *> extension_cstr = extensions
        | std::views::transform(
            [](const std::string &value)
            {
                return value.data();
            })
        | std::ranges::to<std::vector<const char *>>();

    constexpr VkApplicationInfo application_info {
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "HyperEngine",
        .applicationVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .pEngineName = "HyperEngine",
        .engineVersion = VK_MAKE_API_VERSION(0, 1, 0, 0),
        .apiVersion = VK_API_VERSION_1_3,
    };

    constexpr VkDebugUtilsMessengerCreateInfoEXT debug_create_info {
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
            | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
        .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
        .pfnUserCallback = debug_callback,
        .pUserData = nullptr,
    };

    u32 required_extension_count { 0 };
    const char *const *required_instance_extensions = SDL_Vulkan_GetInstanceExtensions(&required_extension_count);

    std::vector<const char *> required_extensions(
        required_instance_extensions, required_instance_extensions + required_extension_count);
    required_extensions.insert(required_extensions.end(), extension_cstr.begin(), extension_cstr.end());

    VkInstanceCreateInfo create_info {
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &application_info,
        .enabledLayerCount = static_cast<u32>(layers_cstr.size()),
        .ppEnabledLayerNames = layers_cstr.data(),
        .enabledExtensionCount = static_cast<u32>(required_extensions.size()),
        .ppEnabledExtensionNames = required_extensions.data(),
    };

    if (extensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        create_info.pNext = &debug_create_info;
    }

    VkInstance raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateInstance(&create_info, nullptr, &raw), vkCreateInstance);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    volkLoadInstance(raw);

    VkDebugUtilsMessengerEXT debug_messenger { VK_NULL_HANDLE };
    if (extensions.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME))
    {
        HE_VK_CHECK(vkCreateDebugUtilsMessengerEXT(raw, &debug_create_info, nullptr, &debug_messenger),
            vkCreateDebugUtilsMessengerEXT);
        HE_ASSERT(debug_messenger != VK_NULL_HANDLE);
    }

    return {
        .raw = raw,
        .debug_messenger = debug_messenger,
    };
}

VKAPI_ATTR VkBool32 VKAPI_CALL VulkanRenderDriver::debug_callback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT message_severity,
    const VkDebugUtilsMessageTypeFlagsEXT,
    const VkDebugUtilsMessengerCallbackDataEXT *callback_data,
    void *)
{
    switch (message_severity)
    {
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: HE_TRACE("{}", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: HE_INFO("{}", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: HE_WARN("{}", callback_data->pMessage); break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: HE_ERROR("{}", callback_data->pMessage); break;
    default: break;
    }

    return VK_FALSE;
}

std::optional<std::unordered_set<std::string>> VulkanRenderDriver::check_device_extensions(
    const VkPhysicalDevice physical_device)
{
    u32 extension_count { 0 };
    HE_VK_CHECK(vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr),
        vkEnumerateDeviceExtensionProperties);

    std::vector<VkExtensionProperties> extensions(extension_count);
    HE_VK_CHECK(vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, extensions.data()),
        vkEnumerateDeviceExtensionProperties);

    // NOTE: This can be extended by turning it into a hashmap and adding a
    // 'required' flag
    std::unordered_set<std::string_view> requested_extensions {};
    requested_extensions.insert(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    std::unordered_set<std::string> enabled_extensions = extensions
        | std::views::filter(
            [&](const VkExtensionProperties &properties)
            {
                const std::string_view name = properties.extensionName;
                return requested_extensions.contains(name);
            })
        | std::views::transform(
            [](const VkExtensionProperties &properties)
            {
                const std::string name = properties.extensionName;
                return name;
            })
        | std::ranges::to<std::unordered_set<std::string>>();

    const bool extensions_satisfied = std::ranges::all_of(requested_extensions,
        [&](const std::string_view extension)
        {
            return enabled_extensions.contains(std::string(extension));
        });

    if (!extensions_satisfied)
    {
        return std::nullopt;
    }

    return enabled_extensions;
}

std::optional<VulkanRenderDriver::PhysicalDevice::FeatureSet> VulkanRenderDriver::check_device_features(
    const VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceVulkan12Features features_12 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .pNext = nullptr,
    };
    VkPhysicalDeviceVulkan13Features features_13 {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .pNext = &features_12,
    };
    VkPhysicalDeviceFeatures2 features {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
        .pNext = &features_13,
        .features = {},
    };
    vkGetPhysicalDeviceFeatures2(physical_device, &features);

    VkPhysicalDeviceFeatures2 enabled_features {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2,
    };

    VkPhysicalDeviceVulkan12Features enabled_features_12 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
    };

    VkPhysicalDeviceVulkan13Features enabled_features_13 = {
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
    };

#define REQUIRE_FEATURE(s, feature)               \
    do                                            \
    {                                             \
        if (s.feature == VK_FALSE)                \
        {                                         \
            return std::nullopt;                  \
        }                                         \
                                                  \
        HE_CONCAT(enabled_, s).feature = VK_TRUE; \
    } while (false)

    REQUIRE_FEATURE(features_12, shaderUniformBufferArrayNonUniformIndexing);
    REQUIRE_FEATURE(features_12, shaderSampledImageArrayNonUniformIndexing);
    REQUIRE_FEATURE(features_12, shaderStorageBufferArrayNonUniformIndexing);
    REQUIRE_FEATURE(features_12, shaderStorageImageArrayNonUniformIndexing);
    REQUIRE_FEATURE(features_12, descriptorBindingUniformBufferUpdateAfterBind);
    REQUIRE_FEATURE(features_12, descriptorBindingSampledImageUpdateAfterBind);
    REQUIRE_FEATURE(features_12, descriptorBindingStorageImageUpdateAfterBind);
    REQUIRE_FEATURE(features_12, descriptorBindingStorageBufferUpdateAfterBind);
    REQUIRE_FEATURE(features_12, descriptorBindingUpdateUnusedWhilePending);
    REQUIRE_FEATURE(features_12, descriptorBindingPartiallyBound);
    REQUIRE_FEATURE(features_12, descriptorBindingVariableDescriptorCount);
    REQUIRE_FEATURE(features_12, runtimeDescriptorArray);
    REQUIRE_FEATURE(features_12, timelineSemaphore);
    REQUIRE_FEATURE(features_13, synchronization2);
    REQUIRE_FEATURE(features_13, dynamicRendering);

#undef REQUIRE_FEATURE

    return PhysicalDevice::FeatureSet {
        .features = enabled_features,
        .features_12 = enabled_features_12,
        .features_13 = enabled_features_13,
    };
}

std::optional<u32> VulkanRenderDriver::check_device_queues(const VkInstance instance, const VkPhysicalDevice physical_device)
{
    u32 queue_family_count { 0 };
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);

    std::vector<VkQueueFamilyProperties> queue_families(queue_family_count);
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_families.data());

    u32 index { 0 };
    for (const VkQueueFamilyProperties &queue_family : queue_families)
    {
        const bool graphics_supported = queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        const bool present_supported = SDL_Vulkan_GetPresentationSupport(instance, physical_device, index);

        if (graphics_supported && present_supported)
        {
            return index;
        }

        ++index;
    }

    return std::nullopt;
}

bool VulkanRenderDriver::check_device_limits(const VkPhysicalDevice physical_device)
{
    VkPhysicalDeviceProperties properties {};
    vkGetPhysicalDeviceProperties(physical_device, &properties);

    const VkPhysicalDeviceLimits limits = properties.limits;
    if (limits.timestampPeriod == 0.0f)
    {
        return false;
    }

    if (limits.timestampComputeAndGraphics == VK_FALSE)
    {
        return false;
    }

    return true;
}

std::string_view VulkanRenderDriver::map_device_type(const VkPhysicalDeviceType device_type)
{
    switch (device_type)
    {
    case VK_PHYSICAL_DEVICE_TYPE_OTHER: return "Other";
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return "Integrated GPU";
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return "Discrete GPU";
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return "Virtual GPU";
    case VK_PHYSICAL_DEVICE_TYPE_CPU: return "CPU";
    default: return "Unknown";
    }
}

u32 VulkanRenderDriver::rate_device_type(const VkPhysicalDeviceType device_type)
{
    switch (device_type)
    {
    case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU: return 4;
    case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU: return 5;
    case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU: return 3;
    case VK_PHYSICAL_DEVICE_TYPE_CPU: return 2;
    case VK_PHYSICAL_DEVICE_TYPE_OTHER:;
    default: return 1;
    }
}

VulkanRenderDriver::PhysicalDevice VulkanRenderDriver::choose_physical_device(const Instance &instance)
{
    u32 physical_device_count { 0 };
    HE_VK_CHECK(vkEnumeratePhysicalDevices(instance.raw, &physical_device_count, nullptr), vkEnumeratePhysicalDevices);

    std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
    HE_VK_CHECK(vkEnumeratePhysicalDevices(instance.raw, &physical_device_count, physical_devices.data()),
        vkEnumeratePhysicalDevices);

    std::unordered_set<std::string> chosen_extensions {};
    PhysicalDevice::FeatureSet chosen_feature_set {};
    u32 chosen_queue_family { 0 };
    VkPhysicalDevice raw { VK_NULL_HANDLE };

    u32 highest_score { 0 };
    for (u32 i { 0 }; i < physical_device_count; ++i)
    {
        const VkPhysicalDevice physical_device = physical_devices[i];

        VkPhysicalDeviceProperties properties {};
        vkGetPhysicalDeviceProperties(physical_device, &properties);

        const std::string_view name = properties.deviceName;
        const std::string_view device_type = map_device_type(properties.deviceType);

        HE_INFO("#{}: {} - {}", i, name, device_type);

        const std::optional<std::unordered_set<std::string>> extensions = check_device_extensions(physical_device);
        if (!extensions.has_value())
        {
            continue;
        }

        const std::optional<PhysicalDevice::FeatureSet> feature_set = check_device_features(physical_device);
        if (!feature_set.has_value())
        {
            continue;
        }

        const std::optional<u32> queue_family = check_device_queues(instance.raw, physical_device);
        if (!queue_family.has_value())
        {
            continue;
        }

        const bool limits_supported = check_device_limits(physical_device);
        if (!limits_supported)
        {
            continue;
        }

        const u32 device_type_score = rate_device_type(properties.deviceType);
        if (device_type_score > highest_score)
        {
            raw = physical_device;
            chosen_extensions = extensions.value();
            chosen_feature_set = feature_set.value();
            chosen_queue_family = queue_family.value();
            highest_score = device_type_score;
        }
    }

    HE_ASSERT(raw != VK_NULL_HANDLE);

    return {
        .extensions = chosen_extensions,
        .feature_set = chosen_feature_set,
        .queue_family = chosen_queue_family,
        .raw = raw,
    };
}

VulkanRenderDriver::Device VulkanRenderDriver::create_device(const Instance &instance, const PhysicalDevice &physical_device)
{
    constexpr f32 queue_priority { 1.0f };
    const VkDeviceQueueCreateInfo queue_create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = physical_device.queue_family,
        .queueCount = 1,
        .pQueuePriorities = &queue_priority,
    };

    const std::vector<const char *> extensions = physical_device.extensions
        | std::views::transform(
            [](const std::string &value)
            {
                return value.data();
            })
        | std::ranges::to<std::vector<const char *>>();

    VkPhysicalDeviceVulkan13Features features_13 = physical_device.feature_set.features_13;
    features_13.pNext = nullptr;

    VkPhysicalDeviceVulkan12Features features_12 = physical_device.feature_set.features_12;
    features_12.pNext = &features_13;

    VkPhysicalDeviceFeatures2 features = physical_device.feature_set.features;
    features.pNext = &features_12;

    VkDeviceCreateInfo create_info {
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = &features,
        .flags = 0,
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queue_create_info,
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = static_cast<u32>(extensions.size()),
        .ppEnabledExtensionNames = extensions.data(),
        .pEnabledFeatures = nullptr,
    };

    if (instance.debug_messenger != VK_NULL_HANDLE)
    {
        create_info.enabledLayerCount = 1;
        create_info.ppEnabledLayerNames = &s_validation_layer;
    }

    VkDevice raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateDevice(physical_device.raw, &create_info, nullptr, &raw), vkCreateDevice);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    volkLoadDevice(raw);

    VkQueue queue { VK_NULL_HANDLE };
    vkGetDeviceQueue(raw, physical_device.queue_family, 0, &queue);
    HE_ASSERT(queue != VK_NULL_HANDLE);

    return {
        .queue_family = physical_device.queue_family,
        .physical_device = physical_device.raw,
        .raw = raw,
        .queue = queue,
    };
}

VmaAllocator VulkanRenderDriver::create_allocator(const Instance &instance, const Device &device)
{
    const VmaVulkanFunctions functions {
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

    const VmaAllocatorCreateInfo allocator_create_info {
        .flags = 0,
        .physicalDevice = device.physical_device,
        .device = device.raw,
        .preferredLargeHeapBlockSize = 0,
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr,
        .pVulkanFunctions = &functions,
        .instance = instance.raw,
        .vulkanApiVersion = VK_API_VERSION_1_3,
        .pTypeExternalMemoryHandleTypes = nullptr,
    };

    VmaAllocator raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vmaCreateAllocator(&allocator_create_info, &raw), vmaCreateAllocator);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    return raw;
}

VkSurfaceKHR VulkanRenderDriver::create_surface(const Instance &instance, void *native_window)
{
    VkSurfaceKHR raw { VK_NULL_HANDLE };
    HE_ASSERT(SDL_Vulkan_CreateSurface(static_cast<SDL_Window *>(native_window), instance.raw, nullptr, &raw));
    HE_ASSERT(raw != VK_NULL_HANDLE);

    return raw;
}

Extent2d VulkanRenderDriver::choose_extent(
    const Device &device, const VkSurfaceKHR surface, const u32 width, const u32 height)
{
    VkSurfaceCapabilitiesKHR capabilities {};
    HE_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical_device, surface, &capabilities),
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR);

    Extent2d extent {
        .width = capabilities.currentExtent.width,
        .height = capabilities.currentExtent.height,
    };

    if (extent.width == std::numeric_limits<u32>::max())
    {
        extent.width = std::clamp(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
    }

    if (extent.height == std::numeric_limits<u32>::max())
    {
        extent.height = std::clamp(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    return extent;
}

VkSurfaceFormatKHR VulkanRenderDriver::choose_surface_format(const Device &device, const VkSurfaceKHR surface)
{
    u32 format_count { 0 };
    HE_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical_device, surface, &format_count, nullptr),
        vkGetPhysicalDeviceSurfaceFormatsKHR);

    std::vector<VkSurfaceFormatKHR> formats(format_count);
    HE_VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(device.physical_device, surface, &format_count, formats.data()),
        vkGetPhysicalDeviceSurfaceFormatsKHR);

    const auto it = std::ranges::find_if(formats,
        [](const VkSurfaceFormatKHR &surface_format)
        {
            return surface_format.format == VK_FORMAT_B8G8R8A8_UNORM
                && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
        });

    return it != formats.end() ? *it : formats[0];
}

VkPresentModeKHR VulkanRenderDriver::choose_present_mode(const Device &device, const VkSurfaceKHR surface)
{
    u32 present_mode_count { 0 };
    HE_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(device.physical_device, surface, &present_mode_count, nullptr),
        vkGetPhysicalDeviceSurfacePresentModesKHR);

    std::vector<VkPresentModeKHR> present_modes(present_mode_count);
    HE_VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(
                    device.physical_device, surface, &present_mode_count, present_modes.data()),
        vkGetPhysicalDeviceSurfacePresentModesKHR);

    const auto it = std::ranges::find_if(present_modes,
        [](const VkPresentModeKHR &present_mode)
        {
            return present_mode == VK_PRESENT_MODE_MAILBOX_KHR;
        });

    return it != present_modes.end() ? *it : present_modes[0];
}

VulkanRenderDriver::Swapchain VulkanRenderDriver::create_swapchain(
    const Device &device, const VkSurfaceKHR surface, const u32 width, const u32 height, const VkSwapchainKHR old)
{
    const Extent2d extent = choose_extent(device, surface, width, height);
    const VkSurfaceFormatKHR format = choose_surface_format(device, surface);
    const VkPresentModeKHR present_mode = choose_present_mode(device, surface);

    VkSurfaceCapabilitiesKHR capabilities {};
    HE_VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.physical_device, surface, &capabilities),
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR);

    u32 min_image_count = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && min_image_count > capabilities.maxImageCount)
    {
        min_image_count = capabilities.maxImageCount;
    }

    const VkSwapchainCreateInfoKHR swapchain_create_info {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surface,
        .minImageCount = min_image_count,
        .imageFormat = format.format,
        .imageColorSpace = format.colorSpace,
        .imageExtent = map_extent_2d(extent),
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = capabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = present_mode,
        .clipped = true,
        .oldSwapchain = old,
    };

    VkSwapchainKHR raw { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateSwapchainKHR(device.raw, &swapchain_create_info, nullptr, &raw), vkCreateSwapchainKHR);
    HE_ASSERT(raw != VK_NULL_HANDLE);

    u32 image_count { 0 };
    HE_VK_CHECK(vkGetSwapchainImagesKHR(device.raw, raw, &image_count, nullptr), vkGetSwapchainImagesKHR);

    std::vector<VkImage> images(image_count);
    HE_VK_CHECK(vkGetSwapchainImagesKHR(device.raw, raw, &image_count, images.data()), vkGetSwapchainImagesKHR);

    const std::vector<TextureId> textures = images
        | std::views::transform(
            [&](const VkImage image)
            {
                VulkanTexture *texture = new VulkanTexture {
                    .raw = image,
                    .allocation = VK_NULL_HANDLE,
                    .layout = VK_IMAGE_LAYOUT_UNDEFINED,
                };
                texture->views = {};

                const TextureId texture_id(texture);

                const TextureDescriptor texture_desc {
                    .label = std::nullopt,
                    .extent = {
                        .width = width,
                        .height = height,
                        .depth = 1,
                    },
                    .mip_levels = 1,
                    .format = map_vk_format(format.format),
                    .dimension = Dimension::D2,
                    .usage = TextureUsage::RenderAttachment,
                };
                texture->desc = texture_desc;

                constexpr VkComponentMapping component_mapping {
                    .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                    .a = VK_COMPONENT_SWIZZLE_IDENTITY,
                };

                const VkImageSubresourceRange subresource_range {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
                };

                const VkImageViewCreateInfo image_view_create_info {
                    .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                    .pNext = nullptr,
                    .flags = 0,
                    .image = texture->raw,
                    .viewType = VK_IMAGE_VIEW_TYPE_2D,
                    .format = format.format,
                    .components = component_mapping,
                    .subresourceRange = subresource_range,
                };

                VkImageView image_view { VK_NULL_HANDLE };
                HE_VK_CHECK(vkCreateImageView(device.raw, &image_view_create_info, nullptr, &image_view), vkCreateImageView);
                HE_ASSERT(image_view != VK_NULL_HANDLE);

                VulkanTextureView *texture_view = new VulkanTextureView {
                    .raw = image_view,
                };
                const TextureViewId texture_view_id(texture_view);

                const TextureViewDescriptor texture_view_desc {
                    .label = std::nullopt,
                    .texture = texture_id,
                    .dimension = ViewDimension::D2,
                    .base_mip_level = 0,
                    .mip_levels = 1,
                    .base_array_layer = 0,
                    .array_layers = 1,
                };
                texture_view->desc = texture_view_desc;

                texture->views.push_back(texture_view_id);

                return texture_id;
            })
        | std::ranges::to<std::vector<TextureId>>();

    return {
        .width = width,
        .height = height,
        .raw = raw,
        .textures = textures,
        .out_of_date = false,
        .texture_index = 0,
    };
}

void VulkanRenderDriver::recreate_swapchain()
{
    wait_idle();

    Swapchain swapchain = create_swapchain(m_device, m_surface, m_swapchain.width, m_swapchain.height, m_swapchain.raw);
    destroy_swapchain();
    m_swapchain = std::move(swapchain);
}

void VulkanRenderDriver::destroy_swapchain()
{
    for (const TextureId texture : m_swapchain.textures)
    {
        destroy_texture(texture);
    }
    vkDestroySwapchainKHR(m_device.raw, m_swapchain.raw, nullptr);
}

VkDescriptorSetLayout VulkanRenderDriver::create_descriptor_set_layout(
    const Device &device, const VkDescriptorType descriptor_type, const u32 descriptor_count)
{
    const VkDescriptorSetLayoutBinding descriptor_set_layout_binding {
        .binding = 0,
        .descriptorType = descriptor_type,
        .descriptorCount = descriptor_count,
        .stageFlags = VK_SHADER_STAGE_ALL,
        .pImmutableSamplers = nullptr,
    };

    constexpr VkDescriptorBindingFlags descriptor_binding_flags = VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT
        | VK_DESCRIPTOR_BINDING_VARIABLE_DESCRIPTOR_COUNT_BIT | VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT;

    VkDescriptorSetLayoutBindingFlagsCreateInfo descriptor_set_layout_binding_flags_info {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .pNext = nullptr,
        .bindingCount = 1,
        .pBindingFlags = &descriptor_binding_flags,
    };

    const VkDescriptorSetLayoutCreateInfo descriptor_set_layout_create_info {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &descriptor_set_layout_binding_flags_info,
        .flags = VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT,
        .bindingCount = 1,
        .pBindings = &descriptor_set_layout_binding,
    };

    VkDescriptorSetLayout descriptor_set_layout { VK_NULL_HANDLE };
    HE_VK_CHECK(vkCreateDescriptorSetLayout(device.raw, &descriptor_set_layout_create_info, nullptr, &descriptor_set_layout),
        vkCreateDescriptorSetLayout);
    HE_ASSERT(descriptor_set_layout != VK_NULL_HANDLE);

    return descriptor_set_layout;
}

VkDescriptorSet VulkanRenderDriver::create_descriptor_set(const Device &device,
    const VkDescriptorPool descriptor_pool,
    const u32 descriptor_count,
    const VkDescriptorSetLayout descriptor_set_layout)
{
    VkDescriptorSetVariableDescriptorCountAllocateInfo descriptor_set_variable_descriptor_count_info {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO,
        .pNext = nullptr,
        .descriptorSetCount = 1,
        .pDescriptorCounts = &descriptor_count,
    };

    const VkDescriptorSetAllocateInfo descriptor_set_allocate_info {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
        .pNext = &descriptor_set_variable_descriptor_count_info,
        .descriptorPool = descriptor_pool,
        .descriptorSetCount = 1,
        .pSetLayouts = &descriptor_set_layout,
    };

    VkDescriptorSet descriptor_set { VK_NULL_HANDLE };
    HE_VK_CHECK(
        vkAllocateDescriptorSets(device.raw, &descriptor_set_allocate_info, &descriptor_set), vkAllocateDescriptorSets);
    HE_ASSERT(descriptor_set != VK_NULL_HANDLE);

    return descriptor_set;
}

VkDescriptorType VulkanRenderDriver::map_descriptor_type(const DescriptorType descriptor_type)
{
    switch (descriptor_type)
    {
    case DescriptorType::StorageBuffer: return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    case DescriptorType::SampledImage: return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
    case DescriptorType::StorageImage: return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    case DescriptorType::Sampler: return VK_DESCRIPTOR_TYPE_SAMPLER;
    default: HE_UNREACHABLE();
    }
}

VulkanRenderDriver::Descriptors VulkanRenderDriver::create_descriptors(const Device &device)
{
    VkPhysicalDeviceProperties properties {};
    vkGetPhysicalDeviceProperties(device.physical_device, &properties);

    std::array<VkDescriptorPoolSize, s_descriptor_count> pool_sizes {};
    for (u8 i { 0 }; i < s_descriptor_count; ++i)
    {
        const VkDescriptorType descriptor_type = map_descriptor_type(static_cast<DescriptorType>(i));

        const u32 limit = [&]()
        {
            switch (descriptor_type)
            {
            case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER: return properties.limits.maxDescriptorSetStorageBuffers;
            case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: return properties.limits.maxDescriptorSetSampledImages;
            case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: return properties.limits.maxDescriptorSetStorageImages;
            case VK_DESCRIPTOR_TYPE_SAMPLER: return properties.limits.maxDescriptorSetSamplers;
            default: HE_UNREACHABLE();
            }
        }();

        const u32 descriptor_count = limit > s_descriptor_limit ? s_descriptor_limit : limit;

        const VkDescriptorPoolSize descriptor_pool_size {
            .type = descriptor_type,
            .descriptorCount = descriptor_count,
        };

        pool_sizes[i] = descriptor_pool_size;
    }

    const VkDescriptorPoolCreateInfo descriptor_pool_create_info {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT,
        .maxSets = static_cast<u32>(s_descriptor_count),
        .poolSizeCount = static_cast<u32>(pool_sizes.size()),
        .pPoolSizes = pool_sizes.data(),
    };

    VkDescriptorPool descriptor_pool { VK_NULL_HANDLE };
    HE_VK_CHECK(
        vkCreateDescriptorPool(device.raw, &descriptor_pool_create_info, nullptr, &descriptor_pool), vkCreateDescriptorPool);
    HE_ASSERT(descriptor_pool != VK_NULL_HANDLE);

    std::array<VkDescriptorSetLayout, s_descriptor_count> descriptor_set_layouts {};
    std::array<VkDescriptorSet, s_descriptor_count> descriptor_sets {};
    for (u8 i { 0 }; i < s_descriptor_count; ++i)
    {
        const VkDescriptorType descriptor_type = map_descriptor_type(static_cast<DescriptorType>(i));
        const u32 descriptor_count = pool_sizes[i].descriptorCount;
        descriptor_set_layouts[i] = create_descriptor_set_layout(device, descriptor_type, descriptor_count);
        descriptor_sets[i] = create_descriptor_set(device, descriptor_pool, descriptor_count, descriptor_set_layouts[i]);
    }

    return {
        .descriptor_pool = descriptor_pool,
        .descriptor_set_layouts = descriptor_set_layouts,
        .descriptor_sets = descriptor_sets,
    };
}

void VulkanRenderDriver::transition_texture_layout(
    const VkCommandBuffer command_buffer, VulkanTexture *texture, const VkImageLayout new_layout)
{
    const VkImageSubresourceRange subresource_range {
        .aspectMask = map_aspect(texture->desc.format),
        .baseMipLevel = 0,
        .levelCount = VK_REMAINING_MIP_LEVELS,
        .baseArrayLayer = 0,
        .layerCount = VK_REMAINING_ARRAY_LAYERS,
    };

    const VkImageMemoryBarrier2 image_memory_barrier {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
        .pNext = nullptr,
        .srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .srcAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT,
        .dstAccessMask = VK_ACCESS_2_MEMORY_WRITE_BIT | VK_ACCESS_2_MEMORY_READ_BIT,
        .oldLayout = texture->layout,
        .newLayout = new_layout,
        .srcQueueFamilyIndex = 0,
        .dstQueueFamilyIndex = 0,
        .image = texture->raw,
        .subresourceRange = subresource_range,
    };

    const VkDependencyInfo dependency_info {
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

    vkCmdPipelineBarrier2(command_buffer, &dependency_info);

    texture->layout = new_layout;
}
