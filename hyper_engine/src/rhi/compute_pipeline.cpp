/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/compute_pipeline.hpp"

#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/shader_module.hpp"

namespace hyper_engine
{
    ComputePipeline::ComputePipeline(const ComputePipelineDescriptor &descriptor)
        : m_label(descriptor.label)
        , m_layout(descriptor.layout)
        , m_shader(descriptor.shader)
    {
    }

    std::string_view ComputePipeline::label() const
    {
        return m_label;
    }

    std::shared_ptr<PipelineLayout> ComputePipeline::layout() const
    {
        return m_layout;
    }

    std::shared_ptr<ShaderModule> ComputePipeline::shader() const
    {
        return m_shader;
    }
} // namespace hyper_engine