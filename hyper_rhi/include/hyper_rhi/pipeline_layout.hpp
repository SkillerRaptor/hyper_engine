/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include <hyper_core/memory.hpp>
#include <hyper_core/types.hpp>

#include "hyper_rhi/resource.hpp"

namespace he
{
    struct PipelineLayoutDescriptor
    {
        std::optional<std::string_view> label = std::nullopt;
        u32 push_constant_size = 0;
    };

    class PipelineLayout : public Resource
    {
    private:
        friend class GraphicsDevice;

    public:
        HE_ALWAYS_INLINE u32 push_constant_size() const { return m_push_constant_size; }

    private:
        PipelineLayout(RefPtr<void> internal_state, const PipelineLayoutDescriptor &desc)
            : Resource(std::move(internal_state))
            , m_push_constant_size(desc.push_constant_size)
        {
        }

    private:
        u32 m_push_constant_size = 0;
    };
} // namespace he
