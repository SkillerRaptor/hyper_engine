/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/validation.hpp"

#include <hyper_core/assertion.hpp>

#include "hyper_rhi/buffer.hpp"
#include "hyper_rhi/compute_pipeline.hpp"
#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/render_pipeline.hpp"
#include "hyper_rhi/sampler.hpp"
#include "hyper_rhi/shader.hpp"
#include "hyper_rhi/texture.hpp"
#include "hyper_rhi/texture_view.hpp"

namespace he
{
    void validate_buffer_descriptor(const BufferDescriptor &desc)
    {
        if (desc.label.has_value())
        {
            HE_ASSERT(!desc.label->empty());
        }

        HE_ASSERT(desc.size != 0);
        HE_ASSERT(desc.usage != BufferUsage::None);

        if (!desc.initial_data.empty())
        {
            HE_ASSERT(desc.initial_data.size() <= desc.size);
        }
    }

    void validate_compute_pipeline_descriptor(const ComputePipelineDescriptor &desc)
    {
        if (desc.label.has_value())
        {
            HE_ASSERT(!desc.label->empty());
        }

        HE_ASSERT(desc.layout != nullptr);
        HE_ASSERT(desc.shader != nullptr);
    }

    void validate_pipeline_layout_descriptor(const PipelineLayoutDescriptor &desc)
    {
        if (desc.label.has_value())
        {
            HE_ASSERT(!desc.label->empty());
        }

        HE_ASSERT((desc.push_constant_size % 4) == 0);
    }

    void validate_render_pipeline_descriptor(const RenderPipelineDescriptor &desc)
    {
        if (desc.label.has_value())
        {
            HE_ASSERT(!desc.label->empty());
        }

        HE_ASSERT(desc.layout != nullptr);
        HE_ASSERT(desc.vertex_shader != nullptr);
        HE_ASSERT(desc.fragment_shader != nullptr);
        HE_ASSERT(!desc.color_attachment_states.empty());

        for (const ColorAttachmentState &color_attachment_state : desc.color_attachment_states)
        {
            HE_ASSERT(color_attachment_state.format != Format::None);
        }

        if (desc.depth_stencil_state.has_value())
        {
            HE_ASSERT(desc.depth_stencil_state->depth_format != Format::None);
        }
    }

    void validate_sampler_descriptor(const SamplerDescriptor &desc)
    {
        if (desc.label.has_value())
        {
            HE_ASSERT(!desc.label->empty());
        }

        HE_ASSERT(desc.min_lod >= 0.0f);
        HE_ASSERT(desc.min_lod <= desc.max_lod);
    }

    void validate_shader_descriptor(const ShaderDescriptor &desc)
    {
        if (desc.label.has_value())
        {
            HE_ASSERT(!desc.label->empty());
        }

        HE_ASSERT(!desc.entry.empty());
        HE_ASSERT(!desc.byte_code.empty());
    }

    void validate_texture_descriptor(const TextureDescriptor &desc)
    {
        if (desc.label.has_value())
        {
            HE_ASSERT(!desc.label->empty());
        }

        HE_ASSERT(desc.extent.width != 0);
        HE_ASSERT(desc.extent.height != 0);
        HE_ASSERT(desc.extent.depth != 0);

        const u32 max_mip_levels
            = std::min(static_cast<u32>(floor(log2(std::max(desc.extent.width, desc.extent.height))) + 1), 16u);
        HE_ASSERT(desc.mip_levels != 0);
        HE_ASSERT(desc.mip_levels <= max_mip_levels);

        if (desc.dimension == Dimension::D1)
        {
            HE_ASSERT(desc.extent.height == 1);
            HE_ASSERT(desc.extent.depth == 1);
        }

        if (desc.dimension == Dimension::D2)
        {
            HE_ASSERT(desc.extent.depth == 1);
        }

        HE_ASSERT(desc.format != Format::None);

        if (desc.format == Format::D16Unorm || desc.format == Format::D32Sfloat || desc.format == Format::S8Uint
            || desc.format == Format::D16UnormS8Uint || desc.format == Format::D24UnormS8Uint
            || desc.format == Format::D32SfloatS8Uint)
        {
            HE_ASSERT(desc.dimension == Dimension::D2);
        }

        HE_ASSERT(desc.usage != TextureUsage::None);

        if (desc.usage.has(TextureUsage::RenderAttachment))
        {
            HE_ASSERT(desc.dimension == Dimension::D2 || desc.dimension == Dimension::D3);
        }
    }

    void validate_texture_view_descriptor(const TextureViewDescriptor &desc)
    {
        if (desc.label.has_value())
        {
            HE_ASSERT(!desc.label->empty());
        }

        HE_ASSERT(desc.texture != nullptr);

        switch (desc.texture->dimension())
        {
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

        HE_ASSERT(desc.base_mip_level <= desc.mip_levels.value_or(desc.texture->mip_levels()));
        if (desc.mip_levels.has_value())
        {
            HE_ASSERT(desc.base_mip_level + desc.mip_levels.value() <= desc.texture->mip_levels());
        }

        HE_ASSERT(desc.base_array_layer <= desc.array_layers.value_or(desc.texture->extent().depth));
        if (desc.array_layers.has_value())
        {
            HE_ASSERT(desc.base_array_layer + desc.array_layers.value() <= desc.texture->extent().depth);
        }
    }
} // namespace he
