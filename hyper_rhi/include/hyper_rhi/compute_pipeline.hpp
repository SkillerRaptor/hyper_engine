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
#include "hyper_rhi/resource.hpp"
#include "hyper_rhi/shader.hpp"

namespace he {

struct ComputePipelineDescriptor {
    std::optional<std::string_view> label = std::nullopt;
    PipelineLayout layout;
    Shader shader;
};

class ComputePipeline : public Resource {
private:
    friend class GraphicsDevice;

public:
    PipelineLayout layout() const { return m_layout; }
    Shader shader() const { return m_shader; }

private:
    ComputePipeline(RefPtr<void> internal_state, const ComputePipelineDescriptor &desc)
        : Resource(std::move(internal_state))
        , m_layout(desc.layout)
        , m_shader(desc.shader)
    {
    }

private:
    PipelineLayout m_layout;
    Shader m_shader;
};

} // namespace he
