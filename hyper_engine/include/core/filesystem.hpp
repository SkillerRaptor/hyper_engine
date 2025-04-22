/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string_view>
#include <vector>

namespace filesystem
{
    std::vector<uint8_t> read_file(std::string_view path);
} // namespace filesystem