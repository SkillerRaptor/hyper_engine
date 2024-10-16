/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

namespace hyper_rhi
{
    struct PipelineLayoutDescriptor
    {
        std::string label;

        uint32_t push_constant_size;
    };

    class PipelineLayout
    {
    public:
        virtual ~PipelineLayout() = default;
    };
} // namespace hyper_rhi
