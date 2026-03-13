/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/graphics_device.hpp"

#include <hyper_core/assertion.hpp>

#include "hyper_rhi/buffer.hpp"
#include "hyper_rhi/compute_pipeline.hpp"
#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/render_pipeline.hpp"
#include "hyper_rhi/sampler.hpp"
#include "hyper_rhi/shader.hpp"
#include "hyper_rhi/texture.hpp"
#include "hyper_rhi/texture_view.hpp"
#include "hyper_rhi/vulkan/graphics_device.hpp"

namespace he {

OwnPtr<GraphicsDevice>
    GraphicsDevice::create(const GraphicsApi graphics_api, const Window &window, const Validation validation_requested)
{
    switch (graphics_api) {
    case GraphicsApi::Vulkan:
        return make_own<VulkanGraphicsDevice>(window, validation_requested);
    default:
        HE_UNREACHABLE();
    }
}

Buffer GraphicsDevice::create_buffer(const BufferDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(desc.size != 0);
    HE_ASSERT(desc.usage != BufferUsage::None);

    if (!desc.initial_data.empty()) {
        HE_ASSERT(desc.initial_data.size() <= desc.size);
    }

    return create_buffer_impl(desc);
}

Shader GraphicsDevice::create_shader(const ShaderDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(!desc.entry.empty());
    HE_ASSERT(!desc.byte_code.empty());

    return create_shader_impl(desc);
}

Sampler GraphicsDevice::create_sampler(const SamplerDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(desc.min_lod >= 0.0f);
    HE_ASSERT(desc.min_lod <= desc.max_lod);

    return create_sampler_impl(desc);
}

Texture GraphicsDevice::create_texture(const TextureDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(desc.extent.width != 0);
    HE_ASSERT(desc.extent.height != 0);
    HE_ASSERT(desc.extent.depth != 0);

    const auto max_mip_levels
        = std::min(static_cast<u32>(floor(log2(std::max(desc.extent.width, desc.extent.height))) + 1), 16u);
    HE_ASSERT(desc.mip_levels != 0);
    HE_ASSERT(desc.mip_levels <= max_mip_levels);

    if (desc.dimension == Dimension::D1) {
        HE_ASSERT(desc.extent.height == 1);
        HE_ASSERT(desc.extent.depth == 1);
    }

    if (desc.dimension == Dimension::D2) {
        HE_ASSERT(desc.extent.depth == 1);
    }

    HE_ASSERT(desc.format != Format::None);

    if (desc.format == Format::D16Unorm || desc.format == Format::D32Sfloat || desc.format == Format::S8Uint
        || desc.format == Format::D16UnormS8Uint || desc.format == Format::D24UnormS8Uint
        || desc.format == Format::D32SfloatS8Uint) {
        HE_ASSERT(desc.dimension == Dimension::D2);
    }

    HE_ASSERT(desc.usage != TextureUsage::None);

    if (desc.usage.has(TextureUsage::RenderAttachment)) {
        HE_ASSERT(desc.dimension == Dimension::D2 || desc.dimension == Dimension::D3);
    }

    return create_texture_impl(desc);
}

TextureView GraphicsDevice::create_texture_view(const TextureViewDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    switch (desc.texture.dimension()) {
    case Dimension::D1:
        HE_ASSERT(desc.dimension == ViewDimension::D1);
        break;
    case Dimension::D2:
        HE_ASSERT(
            desc.dimension == ViewDimension::D2 || desc.dimension == ViewDimension::D2Array
            || desc.dimension == ViewDimension::Cube);
        break;
    case Dimension::D3:
        HE_ASSERT(desc.dimension == ViewDimension::D3);
        break;
    default:
        HE_UNREACHABLE();
    }

    HE_ASSERT(desc.base_mip_level <= desc.mip_levels.value_or(desc.texture.mip_levels()));
    if (desc.mip_levels.has_value()) {
        HE_ASSERT(desc.base_mip_level + desc.mip_levels.value() <= desc.texture.mip_levels());
    }

    HE_ASSERT(desc.base_array_layer <= desc.array_layers.value_or(desc.texture.extent().depth));
    if (desc.array_layers.has_value()) {
        HE_ASSERT(desc.base_array_layer + desc.array_layers.value() <= desc.texture.extent().depth);
    }

    return create_texture_view_impl(desc);
}

PipelineLayout GraphicsDevice::create_pipeline_layout(const PipelineLayoutDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT((desc.push_constant_size % 4) == 0);

    return create_pipeline_layout_impl(desc);
}

ComputePipeline GraphicsDevice::create_compute_pipeline(const ComputePipelineDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    return create_compute_pipeline_impl(desc);
}

RenderPipeline GraphicsDevice::create_render_pipeline(const RenderPipelineDescriptor &desc)
{
    if (desc.label.has_value()) {
        HE_ASSERT(!desc.label->empty());
    }

    HE_ASSERT(!desc.color_attachment_states.empty());

    for (const auto &color_attachment_state : desc.color_attachment_states) {
        HE_ASSERT(color_attachment_state.format != Format::None);
    }

    if (desc.depth_stencil_state.has_value()) {
        HE_ASSERT(desc.depth_stencil_state->depth_format != Format::None);
    }

    return create_render_pipeline_impl(desc);
}

CommandEncoder GraphicsDevice::acquire_command_encoder()
{
    const auto frame_id = m_frame_index % s_frames_in_flight;
    return acquire_command_encoder_impl(frame_id);
}

void GraphicsDevice::submit_command_encoder(CommandEncoder command_encoder)
{
    submit_command_encoder_impl(std::move(command_encoder));
    m_frame_index += 1;
}

} // namespace he
