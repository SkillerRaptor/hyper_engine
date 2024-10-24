/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

namespace hyper_core::string
{
    [[nodiscard]] std::string replace_all(std::string string, std::string_view from, std::string_view to);
} // namespace hyper_core::string