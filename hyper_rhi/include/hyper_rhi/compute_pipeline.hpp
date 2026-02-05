/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include <hyper_core/memory.hpp>

#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/shader.hpp"

namespace he
{
    struct ComputePipelineDescriptor
    {
        std::optional<std::string_view> label = std::nullopt;
        RefPtr<PipelineLayout> layout = nullptr;
        RefPtr<Shader> shader = nullptr;
    };

    class ComputePipeline
    {
    public:
        virtual ~ComputePipeline() = default;

        HE_ALWAYS_INLINE RefPtr<PipelineLayout> layout() const { return m_layout; }
        HE_ALWAYS_INLINE RefPtr<Shader> shader() const { return m_shader; }

    protected:
        explicit ComputePipeline(const ComputePipelineDescriptor &desc)
            : m_layout(desc.layout)
            , m_shader(desc.shader)
        {
        }

    protected:
        RefPtr<PipelineLayout> m_layout = nullptr;
        RefPtr<Shader> m_shader = nullptr;
    };
} // namespace he
