/*
 * Copyright (const c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "render/vulkan/vulkan_render_conversion.hpp"

#include "core/assertion.hpp"

// Buffer
VkBufferUsageFlags map_buffer_usage(const BufferUsage buffer_usage_flags)
{
    VkBufferUsageFlags usage_flags { VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT };

    if ((buffer_usage_flags & BufferUsage::Index) == BufferUsage::Index)
    {
        usage_flags |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    }

    if ((buffer_usage_flags & BufferUsage::Indirect) == BufferUsage::Indirect)
    {
        usage_flags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    }

    if ((buffer_usage_flags & BufferUsage::Storage) == BufferUsage::Storage)
    {
        usage_flags |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    }

    return usage_flags;
}

// Sampler
VkSamplerAddressMode map_address_mode(const AddressMode filter)
{
    switch (filter)
    {
    case AddressMode::Repeat: return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case AddressMode::MirroredRepeat: return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    case AddressMode::ClampToEdge: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case AddressMode::ClampToBorder: return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
    case AddressMode::MirrorClampToEdge: return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
    default: HE_UNREACHABLE();
    }
}

VkBorderColor map_border_color(const BorderColor border_color)
{
    switch (border_color)
    {
    case BorderColor::TransparentBlack: return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    case BorderColor::OpaqueBlack: return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
    case BorderColor::OpaqueWhite: return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
    default: HE_UNREACHABLE();
    }
}

VkCompareOp map_compare_operation(const CompareOperation compare_operation)
{
    switch (compare_operation)
    {
    case CompareOperation::Never: return VK_COMPARE_OP_NEVER;
    case CompareOperation::Less: return VK_COMPARE_OP_LESS;
    case CompareOperation::Equal: return VK_COMPARE_OP_EQUAL;
    case CompareOperation::LessEqual: return VK_COMPARE_OP_LESS_OR_EQUAL;
    case CompareOperation::Greater: return VK_COMPARE_OP_GREATER;
    case CompareOperation::NotEqual: return VK_COMPARE_OP_NOT_EQUAL;
    case CompareOperation::GreaterEqual: return VK_COMPARE_OP_GREATER_OR_EQUAL;
    case CompareOperation::Always: return VK_COMPARE_OP_ALWAYS;
    default: HE_UNREACHABLE();
    }
}

VkFilter map_filter(const Filter filter)
{
    switch (filter)
    {
    case Filter::Nearest: return VK_FILTER_NEAREST;
    case Filter::Linear: return VK_FILTER_LINEAR;
    default: HE_UNREACHABLE();
    }
}

VkSamplerMipmapMode map_mipmap_mode(const Filter filter)
{
    switch (filter)
    {
    case Filter::Nearest: return VK_SAMPLER_MIPMAP_MODE_NEAREST;
    case Filter::Linear: return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    default: HE_UNREACHABLE();
    }
}

// Texture / Texture View
VkImageAspectFlags map_aspect(const Format format)
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
    case Format::Rgba64Sfloat: return VK_IMAGE_ASPECT_COLOR_BIT;
    case Format::D16Unorm:
    case Format::D32Sfloat: return VK_IMAGE_ASPECT_DEPTH_BIT;
    case Format::S8Uint: return VK_IMAGE_ASPECT_STENCIL_BIT;
    case Format::D16UnormS8Uint:
    case Format::D24UnormS8Uint:
    case Format::D32SfloatS8Uint: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    case Format::Unknown:
    default: HE_UNREACHABLE();
    }
}

VkImageType map_dimension(const Dimension dimension)
{
    switch (dimension)
    {
    case Dimension::D1: return VK_IMAGE_TYPE_1D;
    case Dimension::D2: return VK_IMAGE_TYPE_2D;
    case Dimension::D3: return VK_IMAGE_TYPE_3D;
    case Dimension::Unknown:
    default: HE_UNREACHABLE();
    }
}

VkFormat map_format(const Format format)
{
    switch (format)
    {
    case Format::Unknown: return VK_FORMAT_UNDEFINED;
    case Format::R8Unorm: return VK_FORMAT_R8_UNORM;
    case Format::R8Snorm: return VK_FORMAT_R8_SNORM;
    case Format::R8Uint: return VK_FORMAT_R8_UINT;
    case Format::R8Sint: return VK_FORMAT_R8_SINT;
    case Format::R8Srgb: return VK_FORMAT_R8_SRGB;
    case Format::Rg8Unorm: return VK_FORMAT_R8G8_UNORM;
    case Format::Rg8Snorm: return VK_FORMAT_R8G8_SNORM;
    case Format::Rg8Uint: return VK_FORMAT_R8G8_UINT;
    case Format::Rg8Sint: return VK_FORMAT_R8G8_SINT;
    case Format::Rg8Srgb: return VK_FORMAT_R8G8_SRGB;
    case Format::Rgb8Unorm: return VK_FORMAT_R8G8B8_UNORM;
    case Format::Rgb8Snorm: return VK_FORMAT_R8G8B8_SNORM;
    case Format::Rgb8Uint: return VK_FORMAT_R8G8B8_UINT;
    case Format::Rgb8Sint: return VK_FORMAT_R8G8B8_SINT;
    case Format::Rgb8Srgb: return VK_FORMAT_R8G8B8_SRGB;
    case Format::Bgr8Unorm: return VK_FORMAT_B8G8R8_UNORM;
    case Format::Bgr8Snorm: return VK_FORMAT_B8G8R8_SNORM;
    case Format::Bgr8Uint: return VK_FORMAT_B8G8R8_UINT;
    case Format::Bgr8Sint: return VK_FORMAT_B8G8R8_SINT;
    case Format::Bgr8Srgb: return VK_FORMAT_B8G8R8_SRGB;
    case Format::Rgba8Unorm: return VK_FORMAT_R8G8B8A8_UNORM;
    case Format::Rgba8Snorm: return VK_FORMAT_R8G8B8A8_SNORM;
    case Format::Rgba8Uint: return VK_FORMAT_R8G8B8A8_UINT;
    case Format::Rgba8Sint: return VK_FORMAT_R8G8B8A8_SINT;
    case Format::Rgba8Srgb: return VK_FORMAT_R8G8B8A8_SRGB;
    case Format::Bgra8Unorm: return VK_FORMAT_B8G8R8A8_UNORM;
    case Format::Bgra8Snorm: return VK_FORMAT_B8G8R8A8_SNORM;
    case Format::Bgra8Uint: return VK_FORMAT_B8G8R8A8_UINT;
    case Format::Bgra8Sint: return VK_FORMAT_B8G8R8A8_SINT;
    case Format::Bgra8Srgb: return VK_FORMAT_B8G8R8A8_SRGB;
    case Format::R16Unorm: return VK_FORMAT_R16_UNORM;
    case Format::R16Snorm: return VK_FORMAT_R16_SNORM;
    case Format::R16Uint: return VK_FORMAT_R16_UINT;
    case Format::R16Sint: return VK_FORMAT_R16_SINT;
    case Format::R16Sfloat: return VK_FORMAT_R16_SFLOAT;
    case Format::Rg16Unorm: return VK_FORMAT_R16G16_UNORM;
    case Format::Rg16Snorm: return VK_FORMAT_R16G16_SNORM;
    case Format::Rg16Uint: return VK_FORMAT_R16G16_UINT;
    case Format::Rg16Sint: return VK_FORMAT_R16G16_SINT;
    case Format::Rg16Sfloat: return VK_FORMAT_R16G16_SFLOAT;
    case Format::Rgb16Unorm: return VK_FORMAT_R16G16B16_UNORM;
    case Format::Rgb16Snorm: return VK_FORMAT_R16G16B16_SNORM;
    case Format::Rgb16Uint: return VK_FORMAT_R16G16B16_UINT;
    case Format::Rgb16Sint: return VK_FORMAT_R16G16B16_SINT;
    case Format::Rgb16Sfloat: return VK_FORMAT_R16G16B16_SFLOAT;
    case Format::Rgba16Unorm: return VK_FORMAT_R16G16B16A16_UNORM;
    case Format::Rgba16Snorm: return VK_FORMAT_R16G16B16A16_SNORM;
    case Format::Rgba16Uint: return VK_FORMAT_R16G16B16A16_UINT;
    case Format::Rgba16Sint: return VK_FORMAT_R16G16B16A16_SINT;
    case Format::Rgba16Sfloat: return VK_FORMAT_R16G16B16A16_SFLOAT;
    case Format::R32Uint: return VK_FORMAT_R32_UINT;
    case Format::R32Sint: return VK_FORMAT_R32_SINT;
    case Format::R32Sfloat: return VK_FORMAT_R32_SFLOAT;
    case Format::Rg32Uint: return VK_FORMAT_R32G32_UINT;
    case Format::Rg32Sint: return VK_FORMAT_R32G32_SINT;
    case Format::Rg32Sfloat: return VK_FORMAT_R32G32_SFLOAT;
    case Format::Rgb32Uint: return VK_FORMAT_R32G32B32_UINT;
    case Format::Rgb32Sint: return VK_FORMAT_R32G32B32_SINT;
    case Format::Rgb32Sfloat: return VK_FORMAT_R32G32B32_SFLOAT;
    case Format::Rgba32Uint: return VK_FORMAT_R32G32B32A32_UINT;
    case Format::Rgba32Sint: return VK_FORMAT_R32G32B32A32_SINT;
    case Format::Rgba32Sfloat: return VK_FORMAT_R32G32B32A32_SFLOAT;
    case Format::R64Uint: return VK_FORMAT_R64_UINT;
    case Format::R64Sint: return VK_FORMAT_R64_SINT;
    case Format::R64Sfloat: return VK_FORMAT_R64_SFLOAT;
    case Format::Rg64Uint: return VK_FORMAT_R64G64_UINT;
    case Format::Rg64Sint: return VK_FORMAT_R64G64_SINT;
    case Format::Rg64Sfloat: return VK_FORMAT_R64G64_SFLOAT;
    case Format::Rgb64Uint: return VK_FORMAT_R64G64B64_UINT;
    case Format::Rgb64Sint: return VK_FORMAT_R64G64B64_SINT;
    case Format::Rgb64Sfloat: return VK_FORMAT_R64G64B64_SFLOAT;
    case Format::Rgba64Uint: return VK_FORMAT_R64G64B64A64_UINT;
    case Format::Rgba64Sint: return VK_FORMAT_R64G64B64A64_SINT;
    case Format::Rgba64Sfloat: return VK_FORMAT_R64G64B64A64_SFLOAT;
    case Format::D16Unorm: return VK_FORMAT_D16_UNORM;
    case Format::D32Sfloat: return VK_FORMAT_D32_SFLOAT;
    case Format::S8Uint: return VK_FORMAT_S8_UINT;
    case Format::D16UnormS8Uint: return VK_FORMAT_D16_UNORM_S8_UINT;
    case Format::D24UnormS8Uint: return VK_FORMAT_D24_UNORM_S8_UINT;
    case Format::D32SfloatS8Uint: return VK_FORMAT_D32_SFLOAT_S8_UINT;
    default: HE_UNREACHABLE();
    }
}

VkImageUsageFlags map_texture_usage(const TextureUsage texture_usage_flags, const Format format)
{
    VkImageUsageFlags usage { VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
        | VK_IMAGE_USAGE_SAMPLED_BIT };

    if ((texture_usage_flags & TextureUsage::Storage) == TextureUsage::Storage)
    {
        usage |= VK_IMAGE_USAGE_STORAGE_BIT;
    }

    if ((texture_usage_flags & TextureUsage::RenderAttachment) == TextureUsage::RenderAttachment)
    {
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
            case Format::Rgba64Sfloat: usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; break;
            case Format::D16Unorm:
            case Format::D32Sfloat:
            case Format::S8Uint:
            case Format::D16UnormS8Uint:
            case Format::D24UnormS8Uint:
            case Format::D32SfloatS8Uint: usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT; break;
            case Format::Unknown:
            default: HE_UNREACHABLE();
            }
        }
    }

    return usage;
}

VkImageViewType map_view_dimension(const ViewDimension view_dimension)
{
    switch (view_dimension)
    {
    case ViewDimension::D1: return VK_IMAGE_VIEW_TYPE_1D;
    case ViewDimension::D2: return VK_IMAGE_VIEW_TYPE_2D;
    case ViewDimension::D2Array: return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case ViewDimension::D3: return VK_IMAGE_VIEW_TYPE_3D;
    case ViewDimension::Cube: return VK_IMAGE_VIEW_TYPE_CUBE;
    case ViewDimension::Unknown:
    default: HE_UNREACHABLE();
    }
}

Format map_vk_format(const VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_UNDEFINED: return Format::Unknown;
    case VK_FORMAT_R8_UNORM: return Format::R8Unorm;
    case VK_FORMAT_R8_SNORM: return Format::R8Snorm;
    case VK_FORMAT_R8_UINT: return Format::R8Uint;
    case VK_FORMAT_R8_SINT: return Format::R8Sint;
    case VK_FORMAT_R8_SRGB: return Format::R8Srgb;
    case VK_FORMAT_R8G8_UNORM: return Format::Rg8Unorm;
    case VK_FORMAT_R8G8_SNORM: return Format::Rg8Snorm;
    case VK_FORMAT_R8G8_UINT: return Format::Rg8Uint;
    case VK_FORMAT_R8G8_SINT: return Format::Rg8Sint;
    case VK_FORMAT_R8G8_SRGB: return Format::Rg8Srgb;
    case VK_FORMAT_R8G8B8_UNORM: return Format::Rgb8Unorm;
    case VK_FORMAT_R8G8B8_SNORM: return Format::Rgb8Snorm;
    case VK_FORMAT_R8G8B8_UINT: return Format::Rgb8Uint;
    case VK_FORMAT_R8G8B8_SINT: return Format::Rgb8Sint;
    case VK_FORMAT_R8G8B8_SRGB: return Format::Rgb8Srgb;
    case VK_FORMAT_B8G8R8_UNORM: return Format::Bgr8Unorm;
    case VK_FORMAT_B8G8R8_SNORM: return Format::Bgr8Snorm;
    case VK_FORMAT_B8G8R8_UINT: return Format::Bgr8Uint;
    case VK_FORMAT_B8G8R8_SINT: return Format::Bgr8Sint;
    case VK_FORMAT_B8G8R8_SRGB: return Format::Bgr8Srgb;
    case VK_FORMAT_R8G8B8A8_UNORM: return Format::Rgba8Unorm;
    case VK_FORMAT_R8G8B8A8_SNORM: return Format::Rgba8Snorm;
    case VK_FORMAT_R8G8B8A8_UINT: return Format::Rgba8Uint;
    case VK_FORMAT_R8G8B8A8_SINT: return Format::Rgba8Sint;
    case VK_FORMAT_R8G8B8A8_SRGB: return Format::Rgba8Srgb;
    case VK_FORMAT_B8G8R8A8_UNORM: return Format::Bgra8Unorm;
    case VK_FORMAT_B8G8R8A8_SNORM: return Format::Bgra8Snorm;
    case VK_FORMAT_B8G8R8A8_UINT: return Format::Bgra8Uint;
    case VK_FORMAT_B8G8R8A8_SINT: return Format::Bgra8Sint;
    case VK_FORMAT_B8G8R8A8_SRGB: return Format::Bgra8Srgb;
    case VK_FORMAT_R16_UNORM: return Format::R16Unorm;
    case VK_FORMAT_R16_SNORM: return Format::R16Snorm;
    case VK_FORMAT_R16_UINT: return Format::R16Uint;
    case VK_FORMAT_R16_SINT: return Format::R16Sint;
    case VK_FORMAT_R16_SFLOAT: return Format::R16Sfloat;
    case VK_FORMAT_R16G16_UNORM: return Format::Rg16Unorm;
    case VK_FORMAT_R16G16_SNORM: return Format::Rg16Snorm;
    case VK_FORMAT_R16G16_UINT: return Format::Rg16Uint;
    case VK_FORMAT_R16G16_SINT: return Format::Rg16Sint;
    case VK_FORMAT_R16G16_SFLOAT: return Format::Rg16Sfloat;
    case VK_FORMAT_R16G16B16_UNORM: return Format::Rgb16Unorm;
    case VK_FORMAT_R16G16B16_SNORM: return Format::Rgb16Snorm;
    case VK_FORMAT_R16G16B16_UINT: return Format::Rgb16Uint;
    case VK_FORMAT_R16G16B16_SINT: return Format::Rgb16Sint;
    case VK_FORMAT_R16G16B16_SFLOAT: return Format::Rgb16Sfloat;
    case VK_FORMAT_R16G16B16A16_UNORM: return Format::Rgba16Unorm;
    case VK_FORMAT_R16G16B16A16_SNORM: return Format::Rgba16Snorm;
    case VK_FORMAT_R16G16B16A16_UINT: return Format::Rgba16Uint;
    case VK_FORMAT_R16G16B16A16_SINT: return Format::Rgba16Sint;
    case VK_FORMAT_R16G16B16A16_SFLOAT: return Format::Rgba16Sfloat;
    case VK_FORMAT_R32_UINT: return Format::R32Uint;
    case VK_FORMAT_R32_SINT: return Format::R32Sint;
    case VK_FORMAT_R32_SFLOAT: return Format::R32Sfloat;
    case VK_FORMAT_R32G32_UINT: return Format::Rg32Uint;
    case VK_FORMAT_R32G32_SINT: return Format::Rg32Sint;
    case VK_FORMAT_R32G32_SFLOAT: return Format::Rg32Sfloat;
    case VK_FORMAT_R32G32B32_UINT: return Format::Rgb32Uint;
    case VK_FORMAT_R32G32B32_SINT: return Format::Rgb32Sint;
    case VK_FORMAT_R32G32B32_SFLOAT: return Format::Rgb32Sfloat;
    case VK_FORMAT_R32G32B32A32_UINT: return Format::Rgba32Uint;
    case VK_FORMAT_R32G32B32A32_SINT: return Format::Rgba32Sint;
    case VK_FORMAT_R32G32B32A32_SFLOAT: return Format::Rgba32Sfloat;
    case VK_FORMAT_R64_UINT: return Format::R64Uint;
    case VK_FORMAT_R64_SINT: return Format::R64Sint;
    case VK_FORMAT_R64_SFLOAT: return Format::R64Sfloat;
    case VK_FORMAT_R64G64_UINT: return Format::Rg64Uint;
    case VK_FORMAT_R64G64_SINT: return Format::Rg64Sint;
    case VK_FORMAT_R64G64_SFLOAT: return Format::Rg64Sfloat;
    case VK_FORMAT_R64G64B64_UINT: return Format::Rgb64Uint;
    case VK_FORMAT_R64G64B64_SINT: return Format::Rgb64Sint;
    case VK_FORMAT_R64G64B64_SFLOAT: return Format::Rgb64Sfloat;
    case VK_FORMAT_R64G64B64A64_UINT: return Format::Rgba64Uint;
    case VK_FORMAT_R64G64B64A64_SINT: return Format::Rgba64Sint;
    case VK_FORMAT_R64G64B64A64_SFLOAT: return Format::Rgba64Sfloat;
    case VK_FORMAT_D16_UNORM: return Format::D16Unorm;
    case VK_FORMAT_D32_SFLOAT: return Format::D32Sfloat;
    case VK_FORMAT_S8_UINT: return Format::S8Uint;
    case VK_FORMAT_D16_UNORM_S8_UINT: return Format::D16UnormS8Uint;
    case VK_FORMAT_D24_UNORM_S8_UINT: return Format::D24UnormS8Uint;
    case VK_FORMAT_D32_SFLOAT_S8_UINT: return Format::D32SfloatS8Uint;
    default: HE_UNREACHABLE();
    }
}

// Pipeline Layout / Compute Pipeline / Render Pipeline
VkBlendFactor map_blend_factor(const BlendFactor blend_factor)
{
    switch (blend_factor)
    {
    case BlendFactor::Zero: return VK_BLEND_FACTOR_ZERO;
    case BlendFactor::One: return VK_BLEND_FACTOR_ONE;
    case BlendFactor::SrcColor: return VK_BLEND_FACTOR_SRC_COLOR;
    case BlendFactor::OneMinusSrcColor: return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
    case BlendFactor::DstColor: return VK_BLEND_FACTOR_DST_COLOR;
    case BlendFactor::OneMinusDstColor: return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
    case BlendFactor::SrcAlpha: return VK_BLEND_FACTOR_SRC_ALPHA;
    case BlendFactor::OneMinusSrcAlpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    case BlendFactor::DstAlpha: return VK_BLEND_FACTOR_DST_ALPHA;
    case BlendFactor::OneMinusDstAlpha: return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
    case BlendFactor::ConstantColor: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case BlendFactor::OneMinusConstantColor: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
    case BlendFactor::ConstantAlpha: return VK_BLEND_FACTOR_CONSTANT_ALPHA;
    case BlendFactor::OneMinusConstantAlpha: return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA;
    case BlendFactor::SrcAlphaSaturate: return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
    case BlendFactor::Src1Color: return VK_BLEND_FACTOR_SRC1_COLOR;
    case BlendFactor::OneMinusSrc1Color: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
    case BlendFactor::Src1Alpha: return VK_BLEND_FACTOR_SRC1_ALPHA;
    case BlendFactor::OneMinusSrc1Alpha: return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
    default: HE_UNREACHABLE();
    }
}

VkBlendOp map_blend_operation(const BlendOperation blend_operation)
{
    switch (blend_operation)
    {
    case BlendOperation::Add: return VK_BLEND_OP_ADD;
    case BlendOperation::Subtract: return VK_BLEND_OP_SUBTRACT;
    case BlendOperation::ReverseSubtract: return VK_BLEND_OP_REVERSE_SUBTRACT;
    case BlendOperation::Min: return VK_BLEND_OP_MIN;
    case BlendOperation::Max: return VK_BLEND_OP_MAX;
    default: HE_UNREACHABLE();
    }
}

VkColorComponentFlags map_color_writes(const ColorWrites color_writes)
{
    VkColorComponentFlags color_component_flags { 0 };

    if ((color_writes & ColorWrites::R) == ColorWrites::R)
    {
        color_component_flags |= VK_COLOR_COMPONENT_R_BIT;
    }

    if ((color_writes & ColorWrites::G) == ColorWrites::G)
    {
        color_component_flags |= VK_COLOR_COMPONENT_G_BIT;
    }

    if ((color_writes & ColorWrites::B) == ColorWrites::B)
    {
        color_component_flags |= VK_COLOR_COMPONENT_B_BIT;
    }

    if ((color_writes & ColorWrites::A) == ColorWrites::A)
    {
        color_component_flags |= VK_COLOR_COMPONENT_A_BIT;
    }

    return color_component_flags;
}

VkCullModeFlags map_cull_mode(const Face face)
{
    switch (face)
    {
    case Face::None: return VK_CULL_MODE_NONE;
    case Face::Front: return VK_CULL_MODE_FRONT_BIT;
    case Face::Back: return VK_CULL_MODE_BACK_BIT;
    default: HE_UNREACHABLE();
    }
}

VkFrontFace map_front_face(const FrontFace front_face)
{
    switch (front_face)
    {
    case FrontFace::CounterClockwise: return VK_FRONT_FACE_COUNTER_CLOCKWISE;
    case FrontFace::Clockwise: return VK_FRONT_FACE_CLOCKWISE;
    default: HE_UNREACHABLE();
    }
}

VkAttachmentLoadOp map_load_operation(const LoadOperation load_operation)
{
    switch (load_operation)
    {
    case LoadOperation::Clear: return VK_ATTACHMENT_LOAD_OP_CLEAR;
    case LoadOperation::Load: return VK_ATTACHMENT_LOAD_OP_LOAD;
    case LoadOperation::DontCare: return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    default: HE_UNREACHABLE();
    }
}

VkPolygonMode map_polygon_mode(const PolygonMode polygon_mode)
{
    switch (polygon_mode)
    {
    case PolygonMode::Fill: return VK_POLYGON_MODE_FILL;
    case PolygonMode::Line: return VK_POLYGON_MODE_LINE;
    case PolygonMode::Point: return VK_POLYGON_MODE_POINT;
    default: HE_UNREACHABLE();
    }
}

VkPrimitiveTopology map_primitive_topology(const PrimitiveTopology primitive_topology)
{
    switch (primitive_topology)
    {
    case PrimitiveTopology::PointList: return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
    case PrimitiveTopology::LineList: return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
    case PrimitiveTopology::LineStrip: return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
    case PrimitiveTopology::TriangleList: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    case PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    case PrimitiveTopology::TriangleFan: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    default: HE_UNREACHABLE();
    }
}

VkAttachmentStoreOp map_store_operation(const StoreOperation store_operation)
{
    switch (store_operation)
    {
    case StoreOperation::None: return VK_ATTACHMENT_STORE_OP_NONE;
    case StoreOperation::Store: return VK_ATTACHMENT_STORE_OP_STORE;
    case StoreOperation::DontCare: return VK_ATTACHMENT_STORE_OP_DONT_CARE;
    default: HE_UNREACHABLE();
    }
}

VkExtent2D map_extent_2d(const Extent2d extent)
{
    return {
        .width = extent.width,
        .height = extent.height,
    };
}

VkExtent3D map_extent_3d(const Extent3d extent)
{
    return {
        .width = extent.width,
        .height = extent.height,
        .depth = extent.depth,
    };
}

VkOffset2D map_offset_2d(const Offset2d offset)
{
    return {
        .x = offset.x,
        .y = offset.y,
    };
}

VkOffset3D map_offset_3d(const Offset3d offset)
{
    return {
        .x = offset.x,
        .y = offset.y,
        .z = offset.z,
    };
}
