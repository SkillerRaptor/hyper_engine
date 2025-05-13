/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <volk.h>

#include "systems/render/render_types.hpp"

Format map_vk_format(VkFormat format);

VkAttachmentLoadOp map_attachment_load_operation(LoadOperation load_operation);
VkAttachmentStoreOp map_attachment_store_operation(StoreOperation store_operation);

VkBlendFactor map_blend_factor(BlendFactor blend_factor);
VkBlendOp map_blend_operation(BlendOperation blend_operation);
VkBorderColor map_border_color(BorderColor border_color);
VkBufferUsageFlags map_buffer_usage_flags(BitFlags<BufferUsage> buffer_usage_flags);

VkColorComponentFlags map_color_component_flags(BitFlags<ColorWrites> color_writes);
VkCompareOp map_compare_operation(CompareOperation compare_operation);
VkCullModeFlags map_cull_mode_flags(Face face);

VkFilter map_filter(Filter filter);
VkFormat map_format(Format format);
VkFrontFace map_front_face(FrontFace front_face);

VkImageAspectFlags map_image_aspect_flags(Format format);
VkImageType map_image_type(Dimension dimension);
VkImageUsageFlags map_image_usage_flags(BitFlags<TextureUsage> texture_usage_flags, Format format);
VkImageViewType map_image_view_type(Dimension dimension);

VkPolygonMode map_polygon_mode(PolygonMode polygon_mode);
VkPrimitiveTopology map_primitive_topology(PrimitiveTopology primitive_topology);

VkSamplerAddressMode map_sampler_address_mode(AddressMode filter);
VkSamplerMipmapMode map_sampler_mipmap_mode(Filter filter);