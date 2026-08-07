/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "hyper_core/types.hpp"

namespace he::filesystem {

// TODO: Replace with error types
std::optional<std::vector<u8>> read_to_bytes(std::string_view path);
std::optional<std::string> read_to_string(std::string_view path);

} // namespace he::filesystem
