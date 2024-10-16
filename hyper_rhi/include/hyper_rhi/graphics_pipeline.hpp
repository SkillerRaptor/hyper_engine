/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string>

#include "hyper_rhi/pipeline_layout.hpp"
#include "hyper_rhi/shader_module.hpp"

namespace hyper_rhi
{
    struct GraphicsPipelineDescriptor
    {
        std::string label;

        std::shared_ptr<PipelineLayout> layout = nullptr;
        std::shared_ptr<ShaderModule> vertex_shader = nullptr;
        std::shared_ptr<ShaderModule> fragment_shader = nullptr;
    };

    class GraphicsPipeline
    {
    public:
        virtual ~GraphicsPipeline() = default;
    };
} // namespace hyper_rhi
