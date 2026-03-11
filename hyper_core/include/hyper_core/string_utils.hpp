/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

#include "hyper_core/types.hpp"

namespace he::string_utils {

std::string to_string(std::wstring_view);
std::wstring to_wstring(std::string_view);

static constexpr std::string_view extract_function_name(const std::string_view expression)
{
    usize i = 0;
    while (i < expression.size() && (expression[i] == ' ' || expression[i] == '\t')) {
        ++i;
    }

    const usize name_start = i;
    while (i < expression.size() && expression[i] != '(' && expression[i] != ' ') {
        ++i;
    }

    return expression.substr(name_start, i - name_start);
}

static constexpr std::string_view strip_namespace(const std::string_view function_name)
{
    usize i = function_name.size();

    while (i > 1) {
        if (function_name[i - 1] == ':' && function_name[i - 2] == ':') {
            return function_name.substr(i);
        }

        --i;
    }

    return function_name;
}

} // namespace he::string_utils
