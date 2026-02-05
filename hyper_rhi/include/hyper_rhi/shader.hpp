/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>

#include <hyper_core/types.hpp>

#include "hyper_rhi/definitions.hpp"

namespace he
{
    struct ShaderDescriptor
    {
        std::optional<std::string_view> label = std::nullopt;
        ShaderType type = ShaderType::Compute;
        std::string entry;
        std::span<const u8> byte_code;
    };

    class Shader
    {
    public:
        virtual ~Shader() = default;

        HE_ALWAYS_INLINE ShaderType type() const { return m_type; }
        HE_ALWAYS_INLINE std::string_view entry() const { return m_entry; }

    protected:
        explicit Shader(const ShaderDescriptor &desc)
            : m_type(desc.type)
            , m_entry(desc.entry)
        {
        }

    protected:
        ShaderType m_type = ShaderType::Compute;
        std::string m_entry;
    };
} // namespace he
