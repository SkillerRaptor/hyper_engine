/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/filesystem.hpp"

#include <filesystem>
#include <fstream>

#include <fmt/format.h>

#include "hyper_core/assertion.hpp"

namespace he::filesystem {

std::optional<std::vector<u8>> read_to_bytes(const std::string_view path)
{
    HE_ASSERT(!path.empty());

    const auto file_path = std::filesystem::path(path);
    if (!std::filesystem::exists(file_path)) {
        HE_ERROR("Failed to find file: '{}'", path);
        return std::nullopt;
    }

    auto file = std::ifstream(file_path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        HE_ERROR("Failed to open file: '{}'", path);
        return std::nullopt;
    }

    const auto size = file.tellg();

    auto data = std::vector<u8>(static_cast<usize>(size));
    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char *>(data.data()), size);

    return data;
}

std::optional<std::string> read_to_string(std::string_view path)
{
    HE_ASSERT(!path.empty());

    const auto file_path = std::filesystem::path(path);
    if (!std::filesystem::exists(file_path)) {
        HE_ERROR("Failed to find file: '{}'", path);
        return std::nullopt;
    }

    auto file = std::ifstream(file_path, std::ios::ate);
    if (!file.is_open()) {
        HE_ERROR("Failed to open file: '{}'", path);
        return std::nullopt;
    }

    const auto size = file.tellg();

    auto buffer = std::string(static_cast<usize>(size), '\0');
    file.seekg(0, std::ios::beg);
    file.read(buffer.data(), size);

    return buffer;
}

} // namespace he::filesystem
