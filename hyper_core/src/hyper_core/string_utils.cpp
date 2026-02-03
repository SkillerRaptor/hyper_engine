/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/string_utils.hpp"

namespace he::string_utils
{
    std::wstring to_wstring(const std::string_view string)
    {
        const size_t buffer_size = std::mbstowcs(nullptr, string.data(), 0);
        std::wstring wstring(buffer_size, L'\0');
        std::mbstowcs(wstring.data(), string.data(), buffer_size);
        return wstring;
    }
} // namespace he::string_utils
