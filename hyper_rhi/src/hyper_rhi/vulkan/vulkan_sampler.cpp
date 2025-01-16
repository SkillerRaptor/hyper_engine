/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_sampler.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_rhi/vulkan/vulkan_descriptor_manager.hpp"
#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"
#include "hyper_rhi/vulkan/vulkan_render_pipeline.hpp"

namespace hyper_engine
{
    std::shared_ptr<Sampler> VulkanGraphicsDevice::create_sampler_platform(const SamplerDescriptor &descriptor, ResourceHandle handle)
    {
        const VkFilter mag_filter = VulkanSampler::get_filter(descriptor.mag_filter);
        const VkFilter min_filter = VulkanSampler::get_filter(descriptor.min_filter);
        const VkSamplerMipmapMode mipmap_filter = VulkanSampler::get_sampler_mipmap_mode(descriptor.mipmap_filter);
        const VkSamplerAddressMode address_mode_u = VulkanSampler::get_sampler_address_mode(descriptor.address_mode_u);
        const VkSamplerAddressMode address_mode_v = VulkanSampler::get_sampler_address_mode(descriptor.address_mode_v);
        const VkSamplerAddressMode address_mode_w = VulkanSampler::get_sampler_address_mode(descriptor.address_mode_w);
        const VkCompareOp compare_operation = VulkanRenderPipeline::get_compare_operation(descriptor.compare_operation);
        const VkBorderColor border_color = VulkanSampler::get_border_color(descriptor.border_color);
        const VkSamplerCreateInfo sampler_create_info = {
            .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .magFilter = mag_filter,
            .minFilter = min_filter,
            .mipmapMode = mipmap_filter,
            .addressModeU = address_mode_u,
            .addressModeV = address_mode_v,
            .addressModeW = address_mode_w,
            .mipLodBias = descriptor.mip_lod_bias,
            .anisotropyEnable = VK_FALSE,
            .maxAnisotropy = 0.0,
            .compareEnable = VK_TRUE,
            .compareOp = compare_operation,
            .minLod = descriptor.min_lod,
            .maxLod = descriptor.max_lod,
            .borderColor = border_color,
            .unnormalizedCoordinates = VK_FALSE,
        };

        VkSampler sampler = VK_NULL_HANDLE;
        HE_VK_CHECK(vkCreateSampler(m_device, &sampler_create_info, nullptr, &sampler));

        HE_ASSERT(sampler != VK_NULL_HANDLE);

        set_object_name(sampler, ObjectType::Sampler, descriptor.label);

        return std::make_shared<VulkanSampler>(descriptor, handle, *this, sampler);
    }

    VulkanSampler::VulkanSampler(
        const SamplerDescriptor &descriptor,
        const ResourceHandle handle,
        VulkanGraphicsDevice &graphics_device,
        const VkSampler sampler)
        : Sampler(descriptor, handle)
        , m_graphics_device(graphics_device)
        , m_sampler(sampler)
    {
    }

    VulkanSampler::~VulkanSampler()
    {
        m_graphics_device.resource_queue().samplers.emplace_back(m_sampler);
    }

    VkSampler VulkanSampler::sampler() const
    {
        return m_sampler;
    }

    VkFilter VulkanSampler::get_filter(const Filter filter)
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

    VkSamplerMipmapMode VulkanSampler::get_sampler_mipmap_mode(const Filter filter)
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

    VkSamplerAddressMode VulkanSampler::get_sampler_address_mode(const AddressMode filter)
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

    VkBorderColor VulkanSampler::get_border_color(const BorderColor border_color)
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
} // namespace hyper_engine