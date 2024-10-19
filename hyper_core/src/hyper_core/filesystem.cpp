/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/filesystem.hpp"

#include <fstream>

namespace hyper_core::filesystem
{
    std::vector<uint8_t> read_file(const std::string &file_path)
    {
        std::ifstream file(file_path, std::ios::binary | std::ios::ate);
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
} // namespace hyper_core::filesystem