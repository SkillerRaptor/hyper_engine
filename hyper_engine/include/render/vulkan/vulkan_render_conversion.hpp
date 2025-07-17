/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "render/definitions.hpp"

// Buffer
VkBufferUsageFlags map_buffer_usage(BufferUsage);

// Sampler
VkSamplerAddressMode map_address_mode(AddressMode);
VkBorderColor map_border_color(BorderColor);
VkCompareOp map_compare_operation(CompareOperation);
VkFilter map_filter(Filter);
VkSamplerMipmapMode map_mipmap_mode(Filter);

// Texture / Texture View
VkImageAspectFlags map_aspect(Format);
VkImageType map_dimension(Dimension);
VkFormat map_format(Format);
VkSampleCountFlagBits map_sample_count(u32);
VkImageUsageFlags map_texture_usage(TextureUsage, Format);
VkImageViewType map_view_dimension(ViewDimension);
Format map_vk_format(VkFormat);

// Pipeline Layout / Compute Pipeline / Render Pipeline
VkBlendFactor map_blend_factor(BlendFactor);
VkBlendOp map_blend_operation(BlendOperation);
VkColorComponentFlags map_color_writes(ColorWrites);
VkCullModeFlags map_cull_mode(Face);
VkFrontFace map_front_face(FrontFace);
VkAttachmentLoadOp map_load_operation(LoadOperation);
VkPolygonMode map_polygon_mode(PolygonMode);
VkPrimitiveTopology map_primitive_topology(PrimitiveTopology);
VkAttachmentStoreOp map_store_operation(StoreOperation);

// Others
VkExtent2D map_extent_2d(Extent2d);
VkExtent3D map_extent_3d(Extent3d);
VkOffset2D map_offset_2d(Offset2d);
VkOffset3D map_offset_3d(Offset3d);
