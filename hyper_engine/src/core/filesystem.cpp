/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "core/filesystem.hpp"

#include <fstream>

namespace filesystem
{
    std::vector<uint8_t> read_file(const std::string_view path)
    {
        std::ifstream file { path.data(), std::ios::binary | std::ios::ate };
        if (!file.is_open())
        {
            return {};
        }

        const std::ifstream::pos_type size = file.tellg();

        std::vector<uint8_t> data(size);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char *>(data.data()), size);

        return data;
    }
} // namespace filesystem
