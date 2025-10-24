/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <utility>

#define HE_ENABLE_BIT_FLAGS(name)                                                                         \
    inline constexpr name operator|(name lhs, name rhs)                                                   \
    {                                                                                                     \
        return static_cast<name>(std::to_underlying(lhs) | std::to_underlying(rhs));                      \
    }                                                                                                     \
                                                                                                          \
    inline name &operator|=(name &lhs, name rhs)                                                          \
    {                                                                                                     \
        lhs = static_cast<name>(std::to_underlying(lhs) | std::to_underlying(rhs));                       \
        return lhs;                                                                                       \
    }                                                                                                     \
                                                                                                          \
    inline constexpr name operator&(name lhs, name rhs)                                                   \
    {                                                                                                     \
        return static_cast<name>(std::to_underlying(lhs) & std::to_underlying(rhs));                      \
    }                                                                                                     \
                                                                                                          \
    inline name &operator&=(name &lhs, name rhs)                                                          \
    {                                                                                                     \
        lhs = static_cast<name>(std::to_underlying(lhs) & std::to_underlying(rhs));                       \
        return lhs;                                                                                       \
    }                                                                                                     \
                                                                                                          \
    inline constexpr name operator^(name lhs, name rhs)                                                   \
    {                                                                                                     \
        return static_cast<name>(std::to_underlying(lhs) ^ std::to_underlying(rhs));                      \
    }                                                                                                     \
                                                                                                          \
    inline name &operator^=(name &lhs, name rhs)                                                          \
    {                                                                                                     \
        lhs = static_cast<name>(std::to_underlying(lhs) ^ std::to_underlying(rhs));                       \
        return lhs;                                                                                       \
    }                                                                                                     \
                                                                                                          \
    inline constexpr name operator~(name value) { return static_cast<name>(~std::to_underlying(value)); } \
                                                                                                          \
    inline constexpr bool operator!(name value) { return !std::to_underlying(value); }

#define HE_FRIEND_BIT_FLAGS(name)                        \
    friend constexpr name operator|(name lhs, name rhs); \
    friend name &operator|=(name &lhs, name rhs);        \
    friend constexpr name operator&(name lhs, name rhs); \
    friend name &operator&=(name &lhs, name rhs);        \
    friend constexpr name operator^(name lhs, name rhs); \
    friend name &operator^=(name &lhs, name rhs);        \
    friend constexpr name operator~(name value);         \
    friend constexpr bool operator!(name value);

namespace he::bit_flags
{
    template <typename E>
    static constexpr bool has_all_flags(const E flags, const E contains)
    {
        return (std::to_underlying(flags) & std::to_underlying(contains)) == std::to_underlying(contains);
    }

    template <typename E>
    static constexpr bool has_any_flags(const E flags, const E contains)
    {
        return (std::to_underlying(flags) & std::to_underlying(contains)) != 0;
    }

    template <typename E>
    static void add_flags(E &flags, E flags_to_add)
    {
        flags = static_cast<E>(std::to_underlying(flags) | std::to_underlying(flags_to_add));
    }

    template <typename E>
    static void remove_flags(E &flags, E flags_to_remove)
    {
        flags = static_cast<E>(std::to_underlying(flags) & ~std::to_underlying(flags_to_remove));
    }
} // namespace he::bit_flags
