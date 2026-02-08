/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_rhi/forward.hpp"

namespace he
{
    void validate_buffer_descriptor(const BufferDescriptor &);
    void validate_compute_pipeline_descriptor(const ComputePipelineDescriptor &);
    void validate_pipeline_layout_descriptor(const PipelineLayoutDescriptor &);
    void validate_render_pipeline_descriptor(const RenderPipelineDescriptor &);
    void validate_sampler_descriptor(const SamplerDescriptor &);
    void validate_shader_descriptor(const ShaderDescriptor &);
    void validate_texture_descriptor(const TextureDescriptor &);
    void validate_texture_view_descriptor(const TextureViewDescriptor &);
} // namespace he
