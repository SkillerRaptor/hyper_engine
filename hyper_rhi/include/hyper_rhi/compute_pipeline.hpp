/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string>

#include "hyper_rhi/forward.hpp"

namespace hyper_engine
{
    struct ComputePipelineDescriptor
    {
        std::string label;

        std::shared_ptr<PipelineLayout> layout;
        std::shared_ptr<ShaderModule> shader;
    };

    class ComputePipeline
    {
    public:
        virtual ~ComputePipeline() = default;

        std::string_view label() const;
        std::shared_ptr<PipelineLayout> layout() const;
        std::shared_ptr<ShaderModule> shader() const;

    protected:
        explicit ComputePipeline(const ComputePipelineDescriptor &descriptor);

    protected:
        std::string m_label;
        std::shared_ptr<PipelineLayout> m_layout;
        std::shared_ptr<ShaderModule> m_shader;
    };
} // namespace hyper_engine
