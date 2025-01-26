/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "core/string.hpp"

namespace hyper_engine::string
{
    std::wstring to_wstring(const std::string &string)
    {
        const size_t buffer_size = std::mbstowcs(nullptr, string.c_str(), 0);
        std::wstring wstring(buffer_size, L'\0');
        std::mbstowcs(wstring.data(), string.c_str(), buffer_size);
        return wstring;
    }
} // namespace hyper_engine::string