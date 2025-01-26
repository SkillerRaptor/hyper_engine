/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rhi/shader_module.hpp"

namespace hyper_engine
{
    ShaderModule::ShaderModule(const ShaderModuleDescriptor &descriptor)
        : m_label(descriptor.label)
        , m_type(descriptor.type)
        , m_entry_name(descriptor.entry_name)
        , m_bytes(descriptor.bytes)
    {
    }

    std::string_view ShaderModule::label() const
    {
        return m_label;
    }

    ShaderType ShaderModule::type() const
    {
        return m_type;
    }

    std::string_view ShaderModule::entry_name() const
    {
        return m_entry_name;
    }

    const std::vector<uint8_t> &ShaderModule::bytes() const
    {
        return m_bytes;
    }
} // namespace hyper_engine