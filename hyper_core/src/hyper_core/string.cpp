/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/string.hpp"

namespace hyper_core::string
{
    std::string replace_all(std::string string, const std::string_view from, const std::string_view to)
    {
        size_t start_position = 0;
        while ((start_position = string.find(from, start_position)) != std::string::npos)
        {
            string.replace(start_position, from.length(), to);
            start_position += to.length();
        }

        return string;
    }
} // namespace hyper_core::string
