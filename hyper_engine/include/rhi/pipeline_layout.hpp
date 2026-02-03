/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include "core/types.hpp"

namespace he
{
    struct PipelineLayoutDescriptor
    {
        std::optional<std::string_view> label = std::nullopt;
        u32 push_constant_size = 0;
    };

    class PipelineLayout
    {
    public:
        virtual ~PipelineLayout() = default;

        HE_ALWAYS_INLINE u32 push_constant_size() const { return m_push_constant_size; }

    protected:
        explicit PipelineLayout(const PipelineLayoutDescriptor &desc)
            : m_push_constant_size(desc.push_constant_size)
        {
        }

    protected:
        u32 m_push_constant_size = 0;
    };
} // namespace he
