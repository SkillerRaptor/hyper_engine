/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>
#include <vector>

namespace hyper_core::filesystem
{
    std::vector<uint8_t> read_file(const std::string &file_path);
} // namespace hyper_core::filesystem