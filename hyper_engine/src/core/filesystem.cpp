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
    Result<std::vector<u8>, FilesystemError> read_file(const std::string_view path)
    {
        HE_ASSERT(!path.empty());

        const std::filesystem::path file_path(path);
        HE_VERIFY(std::filesystem::exists(file_path), FilesystemError::NotFound);

        std::ifstream file { file_path, std::ios::binary | std::ios::ate };

        const std::ifstream::pos_type size = file.tellg();

        std::vector<u8> data(size);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char *>(data.data()), size);

        return data;
    }
} // namespace he::filesystem
