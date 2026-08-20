/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>

#include "hyper_rhi/types.hpp"

namespace he {

struct ShaderDescriptor {
    std::optional<std::string_view> label = std::nullopt;
    ShaderType type = ShaderType::Compute;
    std::string_view entry;
    std::span<const u8> byte_code = { };
};

class Shader {
public:
    explicit Shader(const ShaderDescriptor &desc)
        : m_type(desc.type)
        , m_entry(desc.entry)
    {
    }

    virtual ~Shader() = default;

    ShaderType type() const { return m_type; }
    std::string_view entry() const { return m_entry; }

private:
    ShaderType m_type = ShaderType::Compute;
    std::string m_entry;
};

} // namespace he
