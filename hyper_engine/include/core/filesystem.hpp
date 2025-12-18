/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "core/types.hpp"

namespace he::filesystem
{
    std::vector<u8> read_to_bytes(std::string_view path);
    std::string read_to_string(std::string_view path);
} // namespace he::filesystem
