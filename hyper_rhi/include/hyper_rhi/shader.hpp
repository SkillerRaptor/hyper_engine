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

#include <hyper_core/key.hpp>
#include <hyper_core/memory.hpp>
#include <hyper_core/types.hpp>

#include "hyper_rhi/definitions.hpp"
#include "hyper_rhi/forward.hpp"

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
        Shader(Key<GraphicsDevice>, RefPtr<void> internal_state, const ShaderDescriptor &desc)
            : m_internal_state(std::move(internal_state))
            , m_type(desc.type)
            , m_entry(desc.entry)
        {
        }

        HE_ALWAYS_INLINE void *internal_state(Key<GraphicsDevice>) const { return m_internal_state.get(); }

        HE_ALWAYS_INLINE ShaderType type() const { return m_type; }
        HE_ALWAYS_INLINE std::string_view entry() const { return m_entry; }

    private:
        RefPtr<void> m_internal_state = nullptr;

        ShaderType m_type = ShaderType::Compute;
        std::string m_entry;
    };
} // namespace he
