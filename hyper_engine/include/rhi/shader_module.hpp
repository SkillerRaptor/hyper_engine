/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>

#include "hyper_rhi/shader_type.hpp"

namespace hyper_engine
{
    struct ShaderModuleDescriptor
    {
        std::string label;
        ShaderType type = ShaderType::None;
        std::string entry_name = "main";
        std::vector<uint8_t> bytes;
    };

    class ShaderModule
    {
    public:
        virtual ~ShaderModule() = default;

        std::string_view label() const;
        ShaderType type() const;
        std::string_view entry_name() const;
        const std::vector<uint8_t> &bytes() const;

    protected:
        explicit ShaderModule(const ShaderModuleDescriptor &descriptor);

    protected:
        std::string m_label;
        ShaderType m_type = ShaderType::None;
        std::string m_entry_name = "main";
        std::vector<uint8_t> m_bytes;
    };
} // namespace hyper_engine
