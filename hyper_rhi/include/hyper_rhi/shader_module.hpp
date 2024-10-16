/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>

namespace hyper_rhi
{
    enum class ShaderType
    {
        None,
        Compute,
        Fragment,
        Vertex
    };

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
    };
} // namespace hyper_rhi
