/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string_view>

#include <hyper_core/types.hpp>

namespace he {

struct PipelineLayoutDescriptor {
    std::optional<std::string_view> label = std::nullopt;
    u32 push_constant_size = 0;
};

class PipelineLayout {
public:
    explicit PipelineLayout(const PipelineLayoutDescriptor &desc)
        : m_push_constant_size(desc.push_constant_size)
    {
    }

    virtual ~PipelineLayout() = default;

    u32 push_constant_size() const { return m_push_constant_size; }

private:
    u32 m_push_constant_size = 0;
};

} // namespace he
