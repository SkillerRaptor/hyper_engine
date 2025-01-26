/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/pipeline_layout.hpp"

namespace hyper_engine
{
    PipelineLayout::PipelineLayout(const PipelineLayoutDescriptor &descriptor)
        : m_label(descriptor.label)
        , m_push_constant_size(descriptor.push_constant_size)
    {
    }

    std::string_view PipelineLayout::label() const
    {
        return m_label;
    }

    uint32_t PipelineLayout::push_constant_size() const
    {
        return m_push_constant_size;
    }
} // namespace hyper_engine