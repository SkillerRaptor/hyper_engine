/*
 * Copyrhs (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

// Credit goes to: https://voithos.io/articles/type-safe-enum-class-bit-flags/

#pragma once

#include <type_traits>
#include <utility>

template <typename T>
    requires std::is_enum_v<T>
constexpr bool s_enable_bit_flags = false;

template <typename T>
    requires s_enable_bit_flags<T>
static constexpr T operator&(const T lhs, const T rhs)
{
    return T(std::to_underlying(lhs) & std::to_underlying(rhs));
}

template <typename T>
    requires s_enable_bit_flags<T>
static constexpr T operator|(const T lhs, const T rhs)
{
    return T(std::to_underlying(lhs) | std::to_underlying(rhs));
}

template <typename T>
    requires s_enable_bit_flags<T>
static constexpr T operator^(const T lhs, const T rhs)
{
    return T(std::to_underlying(lhs) ^ std::to_underlying(rhs));
}

template <typename T>
    requires s_enable_bit_flags<T>
static constexpr T operator~(const T value)
{
    return T(~std::to_underlying(value));
}

#define HE_ENABLE_BIT_FLAGS(name) \
    template <>                   \
    constexpr bool ::s_enable_bit_flags<name> = true
