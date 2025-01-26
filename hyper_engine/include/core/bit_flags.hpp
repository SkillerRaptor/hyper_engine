/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <type_traits>
#include <utility>

namespace hyper_engine
{
    template <typename T>
    class BitFlags
    {
    private:
        using UnderlyingT = std::underlying_type_t<T>;

    public:
        constexpr BitFlags()
            : m_flags(static_cast<UnderlyingT>(0))
        {
        }

        constexpr BitFlags(T flag)
            : m_flags(static_cast<UnderlyingT>(flag))
        {
        }

        constexpr BitFlags(std::initializer_list<T> flags)
            : BitFlags()
        {
            for (T flag : flags)
            {
                m_flags |= static_cast<UnderlyingT>(flag);
            }
        }

        constexpr operator bool() const
        {
            return m_flags != static_cast<UnderlyingT>(0);
        }

        friend constexpr BitFlags operator|(BitFlags left, T right)
        {
            return BitFlags(left.m_flags | static_cast<UnderlyingT>(right));
        }

        friend constexpr BitFlags operator|(BitFlags left, BitFlags right)
        {
            return BitFlags(left.m_flags | right.m_flags);
        }

        friend constexpr BitFlags operator&(BitFlags left, T right)
        {
            return BitFlags(left.m_flags & static_cast<UnderlyingT>(right));
        }

        friend constexpr BitFlags operator&(BitFlags left, BitFlags right)
        {
            return BitFlags(left.m_flags & right.m_flags);
        }

        friend constexpr BitFlags operator^(BitFlags left, T right)
        {
            return BitFlags(left.m_flags ^ static_cast<UnderlyingT>(right));
        }

        friend constexpr BitFlags operator^(BitFlags left, BitFlags right)
        {
            return BitFlags(left.m_flags ^ right.m_flags);
        }

        friend constexpr BitFlags &operator|=(BitFlags &left, T right)
        {
            left.m_flags |= static_cast<UnderlyingT>(right);
            return left;
        }

        friend constexpr BitFlags &operator|=(BitFlags &left, BitFlags right)
        {
            left.m_flags |= right.m_flags;
            return left;
        }

        friend constexpr BitFlags &operator&=(BitFlags &left, T right)
        {
            left.m_flags &= static_cast<UnderlyingT>(right);
            return left;
        }

        friend constexpr BitFlags &operator&=(BitFlags &left, BitFlags right)
        {
            left.m_flags &= right.m_flags;
            return left;
        }

        friend constexpr BitFlags &operator^=(BitFlags &left, T right)
        {
            left.m_flags ^= static_cast<UnderlyingT>(right);
            return left;
        }

        friend constexpr BitFlags &operator^=(BitFlags &left, BitFlags right)
        {
            left.m_flags ^= right.m_flags;
            return left;
        }

        friend constexpr BitFlags operator~(const BitFlags &bf)
        {
            return BitFlags(~bf.m_flags);
        }

        friend constexpr bool operator==(const BitFlags &left, const BitFlags &right)
        {
            return left.m_flags == right.m_flags;
        }

        friend constexpr bool operator!=(const BitFlags &left, const BitFlags &right)
        {
            return left.m_flags != right.m_flags;
        }

    private:
        constexpr BitFlags(UnderlyingT flags)
            : m_flags(flags)
        {
        }

    private:
        UnderlyingT m_flags = 0;
    };
} // namespace hyper_engine