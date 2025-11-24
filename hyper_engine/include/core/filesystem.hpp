/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string_view>
#include <vector>

#include "core/result.hpp"
#include "core/types.hpp"

namespace he::filesystem
{
    enum class FilesystemError
    {
        NotFound,
    };

    Result<std::vector<u8>, FilesystemError> read_file(std::string_view path);
} // namespace he::filesystem
