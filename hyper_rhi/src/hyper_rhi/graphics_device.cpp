/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/graphics_device.hpp"

#include <hyper_core/assertion.hpp>
#include <hyper_platform/window.hpp>

#include "hyper_rhi/buffer.hpp"
#include "hyper_rhi/compute_pipeline.hpp"
#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/render_pipeline.hpp"
#include "hyper_rhi/sampler.hpp"
#include "hyper_rhi/shader.hpp"
#include "hyper_rhi/texture.hpp"
#include "hyper_rhi/texture_view.hpp"
#include "hyper_rhi/types.hpp"
#include "hyper_rhi/vulkan/vulkan_graphics_device.hpp"

namespace he {

std::unique_ptr<GraphicsDevice> GraphicsDevice::create(const GraphicsApi graphics_api, const Window &window)
{
    switch (graphics_api) {
    case GraphicsApi::Vulkan:
        return std::make_unique<VulkanGraphicsDevice>(window);
    default:
        HE_UNREACHABLE();
    }
}

Buffer *GraphicsDevice::create_buffer(const BufferDescriptor &desc)
{
    validate_buffer_descriptor(desc);
    return create_buffer_impl(desc);
}

void GraphicsDevice::destroy_buffer(Buffer *buffer)
{
    HE_ASSERT(buffer != nullptr);
    destroy_buffer_impl(buffer);
}

Shader *GraphicsDevice::create_shader(const ShaderDescriptor &desc)
{
    validate_shader_descriptor(desc);
    return create_shader_impl(desc);
}

void GraphicsDevice::destroy_shader(Shader *shader)
{
    HE_ASSERT(shader != nullptr);
    destroy_shader_impl(shader);
}

Sampler *GraphicsDevice::create_sampler(const SamplerDescriptor &desc)
{
    validate_sampler_descriptor(desc);
    return create_sampler_impl(desc);
}

void GraphicsDevice::destroy_sampler(Sampler *sampler)
{
    HE_ASSERT(sampler != nullptr);
    destroy_sampler_impl(sampler);
}

Texture *GraphicsDevice::create_texture(const TextureDescriptor &desc)
{
    validate_texture_descriptor(desc);
    return create_texture_impl(desc);
}

void GraphicsDevice::destroy_texture(Texture *texture)
{
    HE_ASSERT(texture != nullptr);
    destroy_texture_impl(texture);
}

TextureView *GraphicsDevice::create_texture_view(const TextureViewDescriptor &desc)
{
    validate_texture_view_descriptor(desc);
    return create_texture_view_impl(desc);
}

void GraphicsDevice::destroy_texture_view(TextureView *texture_view)
{
    HE_ASSERT(texture_view != nullptr);
    destroy_texture_view_impl(texture_view);
}

PipelineLayout *GraphicsDevice::create_pipeline_layout(const PipelineLayoutDescriptor &desc)
{
    validate_pipeline_layout_descriptor(desc);
    return create_pipeline_layout_impl(desc);
}

void GraphicsDevice::destroy_pipeline_layout(PipelineLayout *pipeline_layout)
{
    HE_ASSERT(pipeline_layout != nullptr);
    destroy_pipeline_layout_impl(pipeline_layout);
}

ComputePipeline *GraphicsDevice::create_compute_pipeline(const ComputePipelineDescriptor &desc)
{
    validate_compute_pipeline_descriptor(desc);
    return create_compute_pipeline_impl(desc);
}

void GraphicsDevice::destroy_compute_pipeline(ComputePipeline *compute_pipeline)
{
    HE_ASSERT(compute_pipeline != nullptr);
    destroy_compute_pipeline_impl(compute_pipeline);
}

RenderPipeline *GraphicsDevice::create_render_pipeline(const RenderPipelineDescriptor &desc)
{
    validate_render_pipeline_descriptor(desc);
    return create_render_pipeline_impl(desc);
}

void GraphicsDevice::destroy_render_pipeline(RenderPipeline *render_pipeline)
{
    HE_ASSERT(render_pipeline != nullptr);
    destroy_render_pipeline_impl(render_pipeline);
}

void GraphicsDevice::validate_buffer_descriptor(const BufferDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(desc.size != 0);
    HE_ASSERT(desc.usage != BufferUsage::None);

    if (!desc.initial_data.empty()) {
        HE_ASSERT(desc.initial_data.size() <= desc.size);
    }
}

void GraphicsDevice::validate_shader_descriptor(const ShaderDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(!desc.entry.empty());
    HE_ASSERT(!desc.byte_code.empty());
}

void GraphicsDevice::validate_sampler_descriptor(const SamplerDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(desc.min_lod >= 0.0f);
    HE_ASSERT(desc.min_lod <= desc.max_lod);
}

void GraphicsDevice::validate_texture_descriptor(const TextureDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(desc.extent.width != 0);
    HE_ASSERT(desc.extent.height != 0);

    const u32 max_mip_levels
        = std::min(static_cast<u32>(floor(log2(std::max(desc.extent.width, desc.extent.height))) + 1), 16u);
    HE_ASSERT(desc.mip_levels != 0);
    HE_ASSERT(desc.mip_levels <= max_mip_levels);

    HE_ASSERT(desc.array_layers != 0);

    if (desc.dimension == Dimension::D1) {
        HE_ASSERT(desc.extent.height == 1);
    }

    HE_ASSERT(desc.format != Format::None);

    if (desc.format == Format::D16Unorm || desc.format == Format::D32Sfloat || desc.format == Format::S8Uint
        || desc.format == Format::D16UnormS8Uint || desc.format == Format::D24UnormS8Uint
        || desc.format == Format::D32SfloatS8Uint) {
        HE_ASSERT(desc.dimension == Dimension::D2);
    }

    HE_ASSERT(desc.usage != TextureUsage::None);

    if (desc.usage.has(TextureUsage::RenderAttachment)) {
        HE_ASSERT(desc.dimension == Dimension::D2);
    }
}

void GraphicsDevice::validate_texture_view_descriptor(const TextureViewDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    switch (desc.texture->dimension()) {
    case Dimension::D1:
        HE_ASSERT(desc.dimension == ViewDimension::D1);
        break;
    case Dimension::D2:
        HE_ASSERT(desc.dimension == ViewDimension::D2 || desc.dimension == ViewDimension::D2Array);
        break;
    default:
        HE_UNREACHABLE();
    }

    HE_ASSERT(desc.base_mip_level < desc.texture->mip_levels());
    if (desc.mip_levels.has_value()) {
        HE_ASSERT(desc.base_mip_level + desc.mip_levels.value() <= desc.texture->mip_levels());
    }

    HE_ASSERT(desc.base_array_layer < desc.texture->array_layers());
    if (desc.array_layers.has_value()) {
        HE_ASSERT(desc.base_array_layer + desc.array_layers.value() <= desc.texture->array_layers());
    }
}

void GraphicsDevice::validate_pipeline_layout_descriptor(const PipelineLayoutDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT((desc.push_constant_size % 4) == 0);
}

void GraphicsDevice::validate_compute_pipeline_descriptor(const ComputePipelineDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(desc.layout != nullptr);
    HE_ASSERT(desc.shader != nullptr);
}

void GraphicsDevice::validate_render_pipeline_descriptor(const RenderPipelineDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(desc.layout != nullptr);
    HE_ASSERT(desc.vertex_shader != nullptr);
    HE_ASSERT(desc.fragment_shader != nullptr);

    HE_ASSERT(!desc.color_attachment_states.empty());

    for (const ColorAttachmentState &color_attachment_state : desc.color_attachment_states) {
        HE_ASSERT(color_attachment_state.format != Format::None);
    }

    if (desc.depth_stencil_state.has_value()) {
        HE_ASSERT(desc.depth_stencil_state->depth_format != Format::None);
    }
}

} // namespace he
