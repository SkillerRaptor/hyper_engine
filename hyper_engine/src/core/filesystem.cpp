/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "core/filesystem.hpp"

#include <filesystem>
#include <fstream>

#include <fmt/format.h>

namespace he::filesystem
{
    Result<std::vector<uint8_t>> read_file(const std::string_view path)
    {
        const std::filesystem::path file_path(path);
        HE_VERIFY(std::filesystem::exists(file_path), "Failed to find file: {}", path);

        std::ifstream file { file_path, std::ios::binary | std::ios::ate };
        HE_VERIFY(!file.is_open(), "Failed to open file: {}", path);

        const std::ifstream::pos_type size = file.tellg();

        std::vector<uint8_t> data(size);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char *>(data.data()), size);

        return data;
    }
} // namespace he::filesystem
