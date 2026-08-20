/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include "hyper_rhi/forward.hpp"

namespace he {

struct ComputePipelineDescriptor {
    std::optional<std::string_view> label = std::nullopt;
    PipelineLayout *layout = nullptr;
    Shader *shader = nullptr;
};

class ComputePipeline {
public:
    explicit ComputePipeline(const ComputePipelineDescriptor &desc)
        : m_layout(desc.layout)
        , m_shader(desc.shader)
    {
    }

    virtual ~ComputePipeline() = default;

    PipelineLayout *layout() const { return m_layout; }
    Shader *shader() const { return m_shader; }

private:
    PipelineLayout *m_layout = nullptr;
    Shader *m_shader = nullptr;
};

} // namespace he
