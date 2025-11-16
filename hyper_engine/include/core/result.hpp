/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <expected>

#include <fmt/format.h>

namespace he
{
    template <typename T, typename E = std::string>
    using Result = std::expected<T, E>;

#define HE_VERIFY(expression, message, ...)                            \
    do                                                                 \
    {                                                                  \
        [[unlikely]] if (!(expression))                                \
        {                                                              \
            return std::unexpected(fmt::format(message, __VA_ARGS__)); \
        }                                                              \
    } while (false)
} // namespace he
