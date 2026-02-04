/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace he
{
    struct BufferDescriptor;
    struct ComputePipelineDescriptor;
    struct PipelineLayoutDescriptor;
    struct RenderPipelineDescriptor;
    struct SamplerDescriptor;
    struct ShaderDescriptor;
    struct TextureDescriptor;

    bool validate_buffer_descriptor(const BufferDescriptor &desc);
    bool validate_compute_pipeline_descriptor(const ComputePipelineDescriptor &desc);
    bool validate_pipeline_layout_descriptor(const PipelineLayoutDescriptor &desc);
    bool validate_render_pipeline_descriptor(const RenderPipelineDescriptor &desc);
    bool validate_sampler_descriptor(const SamplerDescriptor &desc);
    bool validate_shader_descriptor(const ShaderDescriptor &desc);
    bool validate_texture_descriptor(const TextureDescriptor &desc);
} // namespace he
