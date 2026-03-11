/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/string_utils.hpp"

#include <codecvt>
#include <locale>

namespace he::string_utils {

std::string to_string(const std::wstring_view input)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.to_bytes(input.data(), input.data() + input.size());
}

std::wstring to_wstring(const std::string_view input)
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    return converter.from_bytes(input.data(), input.data() + input.size());
}

} // namespace he::string_utils
