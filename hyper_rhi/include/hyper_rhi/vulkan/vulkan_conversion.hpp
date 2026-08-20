/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "hyper_rhi/buffer.hpp"
#include "hyper_rhi/texture.hpp"

namespace he::conversion {

Format to_format(VkFormat);
VkAttachmentLoadOp to_vk_attachment_load_operation(LoadOperation);
VkAttachmentStoreOp to_vk_attachment_store_operation(StoreOperation);
VkBlendFactor to_vk_blend_factor(BlendFactor);
VkBlendOp to_vk_blend_operation(BlendOperation);
VkBorderColor to_vk_border_color(BorderColor);
VkBufferUsageFlags to_vk_buffer_usage(BitFlags<BufferUsage>);
VkColorComponentFlags to_vk_color_component(BitFlags<ColorWrites>);
VkCompareOp to_vk_compare_operation(CompareOperation);
VkCullModeFlags to_vk_cull_mode(Face);
VkExtent2D to_vk_extent_2d(Extent2d);
VkExtent3D to_vk_extent_3d(Extent3d);
VkFilter to_vk_filter(Filter);
VkFormat to_vk_format(Format);
VkFrontFace to_vk_front_face(FrontFace);
VkImageAspectFlags to_vk_image_aspect(Format);
VkImageType to_vk_image_type(Dimension);
VkImageUsageFlags to_vk_image_usage(BitFlags<TextureUsage>, Format);
VkImageViewType to_vk_image_view_type(ViewDimension);
VkOffset2D to_vk_offset_2d(Offset2d);
VkOffset3D to_vk_offset_3d(Offset3d);
VkPolygonMode to_vk_polygon_mode(PolygonMode);
VkPrimitiveTopology to_vk_primitive_topology(PrimitiveTopology);
VkSampleCountFlagBits to_vk_sample_count(u32);
VkSamplerAddressMode to_vk_sampler_address_mode(AddressMode);
VkSamplerMipmapMode to_vk_sampler_mipmap_mode(Filter);

} // namespace he::conversion
