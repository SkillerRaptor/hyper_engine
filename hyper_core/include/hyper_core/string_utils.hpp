/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

namespace he::string_utils
{
    std::string to_string(std::wstring_view);
    std::wstring to_wstring(std::string_view);
} // namespace he::string_utils
