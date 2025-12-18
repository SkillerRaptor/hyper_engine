/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "core/filesystem.hpp"

#include <filesystem>
#include <fstream>

#include <fmt/format.h>

#include "core/assertion.hpp"

namespace he::filesystem
{
    std::vector<u8> read_to_bytes(const std::string_view path)
    {
        HE_ASSERT(!path.empty());

        const std::filesystem::path file_path(path);
        HE_ASSERT(std::filesystem::exists(file_path));

        std::ifstream file { file_path, std::ios::binary | std::ios::ate };
        HE_ASSERT(file.is_open());

        const std::ifstream::pos_type size = file.tellg();

        std::vector<u8> data(size);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char *>(data.data()), size);

        return data;
    }

    std::string read_to_string(std::string_view path)
    {
        HE_ASSERT(!path.empty());

        const std::filesystem::path file_path(path);
        HE_ASSERT(std::filesystem::exists(file_path));

        std::ifstream file { file_path, std::ios::ate };
        HE_ASSERT(file.is_open());

        const std::ifstream::pos_type size = file.tellg();

        std::string buffer(size, '\0');
        file.seekg(0, std::ios::beg);
        file.read(buffer.data(), size);

        return buffer;
    }

} // namespace he::filesystem
