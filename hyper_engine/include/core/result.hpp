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
    // NOTE: Abstract std::expected, so it can be replaced later
    template <typename T, typename E>
    using Result = std::expected<T, E>;

#define HE_VERIFY(expression, result)          \
    do                                         \
    {                                          \
        [[unlikely]] if (!(expression))        \
        {                                      \
            return std::unexpected { result }; \
        }                                      \
    } while (false)
} // namespace he
