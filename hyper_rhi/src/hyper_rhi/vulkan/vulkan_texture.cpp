/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/vulkan/vulkan_texture.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"
#include "hyper_rhi/vulkan/vulkan_texture_view.hpp"

namespace hyper_engine
{
    RefPtr<Texture> VulkanGraphicsDevice::create_texture_platform(const TextureDescriptor &descriptor)
    {
        return create_texture_internal(descriptor, VK_NULL_HANDLE);
    }

    RefPtr<Texture> VulkanGraphicsDevice::create_texture_internal(const TextureDescriptor &descriptor, const VkImage image)
    {
        if (image != VK_NULL_HANDLE)
        {
            set_object_name(image, ObjectType::Image, descriptor.label);

            return make_ref<VulkanTexture>(descriptor, *this, image, VK_NULL_HANDLE);
        }

        const VkImageType image_type = VulkanTexture::get_image_type(descriptor.dimension);
        const VkFormat format = VulkanTexture::get_format(descriptor.format);
        const VkImageUsageFlags usage = VulkanTexture::get_image_usage_flags(descriptor.usage, descriptor.format);
        const VkImageCreateInfo image_create_info = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .imageType = image_type,
            .format = format,
            .extent =
                {
                    .width = descriptor.width,
                    .height = descriptor.height,
                    .depth = descriptor.depth,
                },
            .mipLevels = descriptor.mip_levels,
            .arrayLayers = descriptor.array_size,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .tiling = VK_IMAGE_TILING_OPTIMAL,
            .usage = usage,
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

        VkImage vk_image = VK_NULL_HANDLE;
        VmaAllocation allocation = VK_NULL_HANDLE;
        HE_VK_CHECK(vmaCreateImage(m_allocator, &image_create_info, &allocation_create_info, &vk_image, &allocation, nullptr));

        HE_ASSERT(vk_image != VK_NULL_HANDLE);
        HE_ASSERT(allocation != VK_NULL_HANDLE);

        set_object_name(vk_image, ObjectType::Image, descriptor.label);

        return make_ref<VulkanTexture>(descriptor, *this, vk_image, allocation);
    }

    VulkanTexture::VulkanTexture(
        const TextureDescriptor &descriptor,
        VulkanGraphicsDevice &graphics_device,
        const VkImage image,
        const VmaAllocation allocation)
        : Texture(descriptor)
        , m_graphics_device(graphics_device)
        , m_image(image)
        , m_allocation(allocation)
    {
    }

    VulkanTexture::~VulkanTexture()
    {
        m_graphics_device.resource_queue().textures.emplace_back(m_image, m_allocation);
    }

    VkImage VulkanTexture::image() const
    {
        return m_image;
    }

    VmaAllocation VulkanTexture::allocation() const
    {
        return m_allocation;
    }

    Format VulkanTexture::format_to_texture_format(const VkFormat format)
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

    VkFormat VulkanTexture::get_format(const Format format)
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

    VkImageType VulkanTexture::get_image_type(const Dimension dimension)
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

    VkImageUsageFlags VulkanTexture::get_image_usage_flags(const BitFlags<TextureUsage> texture_usage_flags, const Format format)
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
} // namespace hyper_engine