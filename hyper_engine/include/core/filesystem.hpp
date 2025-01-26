/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string_view>
#include <vector>

namespace hyper_engine::filesystem
{
    std::vector<uint8_t> read_file(std::string_view path);
} // namespace hyper_engine::filesystem