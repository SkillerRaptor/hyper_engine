/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/compute_pass.hpp"

namespace hyper_engine
{
    ComputePass::ComputePass(const ComputePassDescriptor &descriptor)
        : m_label(descriptor.label)
        , m_label_color(descriptor.label_color)
    {
    }

    std::string_view ComputePass::label() const
    {
        return m_label;
    }

    LabelColor ComputePass::label_color() const
    {
        return m_label_color;
    }
} // namespace hyper_engine