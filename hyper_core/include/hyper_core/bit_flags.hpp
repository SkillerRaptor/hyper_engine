/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>
#include <utility>

namespace he {

template <typename T>
    requires std::is_enum_v<T>
class BitFlags {
private:
    using UnderlyingT = std::underlying_type_t<T>;

public:
    constexpr BitFlags() = default;

    constexpr BitFlags(T flag)
        : m_flags(std::to_underlying(flag))
    {
    }

    constexpr BitFlags(std::initializer_list<T> flags)
    {
        for (const T flag : flags) {
            m_flags |= std::to_underlying(flag);
        }
    }

    constexpr void set(const T value) { m_flags |= std::to_underlying(value); }

    constexpr void remove(const T value) { m_flags &= ~std::to_underlying(value); }

    constexpr void clear() { m_flags = static_cast<UnderlyingT>(0); }

    constexpr bool has(const T value) const { return has_all(value); }

    constexpr bool has_all(const T value) const
    {
        return (m_flags & std::to_underlying(value)) == std::to_underlying(value);
    }

    constexpr bool has_any(const T value) const { return (m_flags & std::to_underlying(value)) != 0; }

    friend constexpr BitFlags operator|(const BitFlags lhs, const T rhs)
    {
        return BitFlags(lhs.m_flags | std::to_underlying(rhs));
    }

    friend constexpr BitFlags operator|(const BitFlags lhs, const BitFlags rhs)
    {
        return BitFlags(lhs.m_flags | rhs.m_flags);
    }

    friend constexpr BitFlags &operator|=(BitFlags &lhs, const T rhs)
    {
        lhs.m_flags |= std::to_underlying(rhs);
        return lhs;
    }

    friend constexpr BitFlags &operator|=(BitFlags &lhs, const BitFlags rhs)
    {
        lhs.m_flags |= rhs.m_flags;
        return lhs;
    }

    friend constexpr BitFlags operator&(const BitFlags lhs, const T rhs)
    {
        return BitFlags(lhs.m_flags & std::to_underlying(rhs));
    }

    friend constexpr BitFlags operator&(const BitFlags lhs, const BitFlags rhs)
    {
        return BitFlags(lhs.m_flags & rhs.m_flags);
    }

    friend constexpr BitFlags &operator&=(BitFlags &lhs, const T rhs)
    {
        lhs.m_flags &= std::to_underlying(rhs);
        return lhs;
    }

    friend constexpr BitFlags &operator&=(BitFlags &lhs, const BitFlags rhs)
    {
        lhs.m_flags &= rhs.m_flags;
        return lhs;
    }

    friend constexpr BitFlags operator^(const BitFlags lhs, const T rhs)
    {
        return BitFlags(lhs.m_flags ^ std::to_underlying(rhs));
    }

    friend constexpr BitFlags operator^(const BitFlags lhs, const BitFlags rhs)
    {
        return BitFlags(lhs.m_flags ^ rhs.m_flags);
    }

    friend constexpr BitFlags &operator^=(BitFlags &lhs, const T rhs)
    {
        lhs.m_flags ^= std::to_underlying(rhs);
        return lhs;
    }

    friend constexpr BitFlags &operator^=(BitFlags &lhs, const BitFlags rhs)
    {
        lhs.m_flags ^= rhs.m_flags;
        return lhs;
    }

    friend constexpr BitFlags operator~(const BitFlags &flags) { return BitFlags(~flags.m_flags); }

    friend constexpr bool operator==(const BitFlags &lhs, const BitFlags &rhs) { return lhs.m_flags == rhs.m_flags; }

private:
    constexpr explicit BitFlags(UnderlyingT flags)
        : m_flags(flags)
    {
    }

private:
    UnderlyingT m_flags = 0;
};

} // namespace he
