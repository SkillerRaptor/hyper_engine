/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/validation.hpp"

#include <hyper_core/logger.hpp>

#include "hyper_rhi/buffer.hpp"
#include "hyper_rhi/compute_pipeline.hpp"
#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/render_pipeline.hpp"
#include "hyper_rhi/sampler.hpp"
#include "hyper_rhi/shader.hpp"
#include "hyper_rhi/texture.hpp"

namespace he
{
    bool validate_buffer_descriptor(const BufferDescriptor &desc)
    {
        if (desc.label.has_value() && desc.label->empty())
        {
            HE_ERROR("Validation failed for buffer descriptor, because the label is empty");
            return false;
        }

        if (desc.size == 0)
        {
            HE_ERROR("Validation failed for buffer descriptor, because the size is 0");
            return false;
        }

        if (desc.usage == BufferUsage::None)
        {
            HE_ERROR("Validation failed for buffer descriptor, because no usage flags were specified");
            return false;
        }

        if (!desc.initial_data.empty() && desc.initial_data.size() > desc.size)
        {
            HE_ERROR(
                "Validation failed for buffer descriptor, because the initial data exceeds the buffer size "
                "(data_size={}, buffer_size={})",
                desc.initial_data.size(),
                desc.size);
            return false;
        }

        return true;
    }

    bool validate_compute_pipeline_descriptor(const ComputePipelineDescriptor &desc)
    {
        if (desc.label.has_value() && desc.label->empty())
        {
            HE_ERROR("Validation failed for compute pipeline descriptor, because the label is empty");
            return false;
        }

        if (desc.layout == nullptr)
        {
            HE_ERROR("Validation failed for compute pipeline descriptor, because the layout is not valid");
            return false;
        }

        if (desc.shader == nullptr)
        {
            HE_ERROR("Validation failed for compute pipeline descriptor, because the shader is not valid");
            return false;
        }

        return true;
    }

    bool validate_pipeline_layout_descriptor(const PipelineLayoutDescriptor &desc)
    {
        if (desc.label.has_value() && desc.label->empty())
        {
            HE_ERROR("Validation failed for pipeline layout descriptor, because the label is empty");
            return false;
        }

        if ((desc.push_constant_size % 4) != 0)
        {
            HE_ERROR("Validation failed for pipeline layout descriptor, because the push constant size is not aligned");
            return false;
        }

        return true;
    }

    bool validate_render_pipeline_descriptor(const RenderPipelineDescriptor &desc)
    {
        if (desc.label.has_value() && desc.label->empty())
        {
            HE_ERROR("Validation failed for render pipeline descriptor, because the label is empty");
            return false;
        }

        if (desc.layout == nullptr)
        {
            return false;
        }

        if (desc.vertex_shader == nullptr)
        {
            return false;
        }

        if (desc.fragment_shader == nullptr)
        {
            return false;
        }

        if (desc.color_attachment_states.empty())
        {
            return false;
        }

        for (const ColorAttachmentState &color_attachment_state : desc.color_attachment_states)
        {
            if (color_attachment_state.format == Format::None)
            {
                return false;
            }
        }

        if (desc.depth_stencil_state.has_value() && desc.depth_stencil_state->depth_format == Format::None)
        {
            return false;
        }

        return true;
    }

    bool validate_sampler_descriptor(const SamplerDescriptor &desc)
    {
        if (desc.label.has_value() && desc.label->empty())
        {
            HE_ERROR("Validation failed for sampler descriptor, because the label is empty");
            return false;
        }

        if (desc.min_lod < 0.0f)
        {
            return false;
        }

        if (desc.max_lod < desc.min_lod)
        {
            return false;
        }

        return true;
    }

    bool validate_shader_descriptor(const ShaderDescriptor &desc)
    {
        if (desc.label.has_value() && desc.label->empty())
        {
            HE_ERROR("Validation failed for shader descriptor, because the label is empty");
            return false;
        }

        if (desc.entry.empty())
        {
            return false;
        }

        if (desc.byte_code.empty())
        {
            return false;
        }

        return true;
    }

    bool validate_texture_descriptor(const TextureDescriptor &desc)
    {
        if (desc.label.has_value() && desc.label->empty())
        {
            HE_ERROR("Validation failed for texture descriptor, because the label is empty");
            return false;
        }

        if (desc.extent.width == 0)
        {
            return false;
        }

        if (desc.extent.height == 0)
        {
            return false;
        }

        if (desc.extent.depth == 0)
        {
            return false;
        }

        const uint32_t max_mip_levels
            = std::min(static_cast<uint32_t>(floor(log2(std::max(desc.extent.width, desc.extent.height))) + 1), 16u);
        if (desc.mip_levels == 0)
        {
            return false;
        }

        if (desc.mip_levels > max_mip_levels)
        {
            return false;
        }

        if (desc.format == Format::None)
        {
            return false;
        }

        if ((desc.format == Format::D16Unorm || desc.format == Format::D32Sfloat || desc.format == Format::S8Uint
             || desc.format == Format::D16UnormS8Uint || desc.format == Format::D24UnormS8Uint
             || desc.format == Format::D32SfloatS8Uint)
            && desc.dimension != Dimension::D2)
        {
            return false;
        }

        if (desc.usage == TextureUsage::None)
        {
            return false;
        }

        if (desc.usage.has(TextureUsage::RenderAttachment)
            && !(desc.dimension == Dimension::D2 || desc.dimension == Dimension::D3))
        {
            return false;
        }

        return true;
    }
} // namespace he
