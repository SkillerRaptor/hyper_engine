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

#include <hyper_core/memory.hpp>
#include <hyper_core/span.hpp>
#include <hyper_core/types.hpp>

#include "hyper_rhi/definitions.hpp"
#include "hyper_rhi/resource.hpp"

namespace he {

struct ShaderDescriptor {
    std::optional<std::string_view> label = std::nullopt;
    ShaderType type = ShaderType::Compute;
    std::string entry;
    ReadonlySpan<u8> byte_code;
};

class Shader : public Resource {
private:
    friend class GraphicsDevice;

public:
    HE_ALWAYS_INLINE ShaderType type() const { return m_type; }
    HE_ALWAYS_INLINE std::string_view entry() const { return m_entry; }

private:
    Shader(RefPtr<void> internal_state, const ShaderDescriptor &desc)
        : Resource(std::move(internal_state))
        , m_type(desc.type)
        , m_entry(desc.entry)
    {
    }

private:
    ShaderType m_type = ShaderType::Compute;
    std::string m_entry;
};

} // namespace he
