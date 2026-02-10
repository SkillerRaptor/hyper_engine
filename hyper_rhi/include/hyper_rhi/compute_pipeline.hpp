/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include <hyper_core/key.hpp>
#include <hyper_core/memory.hpp>

#include "hyper_rhi/forward.hpp"
#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/shader.hpp"

namespace he
{
    struct ComputePipelineDescriptor
    {
        std::optional<std::string_view> label = std::nullopt;
        PipelineLayout layout;
        Shader shader;
    };

    class ComputePipeline
    {
    public:
        ComputePipeline(Key<GraphicsDevice>, RefPtr<void> internal_state, const ComputePipelineDescriptor &desc)
            : m_internal_state(std::move(internal_state))
            , m_layout(desc.layout)
            , m_shader(desc.shader)
        {
        }

        HE_ALWAYS_INLINE void *internal_state(Key<GraphicsDevice>) const { return m_internal_state.get(); }

        HE_ALWAYS_INLINE PipelineLayout layout() const { return m_layout; }
        HE_ALWAYS_INLINE Shader shader() const { return m_shader; }

    private:
        RefPtr<void> m_internal_state = nullptr;

        PipelineLayout m_layout;
        Shader m_shader;
    };
} // namespace he
