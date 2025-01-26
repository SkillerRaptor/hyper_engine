/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace hyper_engine
{
    enum class ShaderType : uint8_t
    {
        None,
        Compute,
        Fragment,
        Vertex
    };
} // namespace hyper_engine