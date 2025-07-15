/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/types.hpp"

class Id
{
public:
    Id() = default;

    explicit Id(const u64 id)
        : m_id(id)
    {
    }

    template <typename T>
    T *as() const
    {
        return reinterpret_cast<T *>(m_id);
    }

    bool is_valid() const { return m_id != 0; }

    u64 get() const { return m_id; }

protected:
    u64 m_id { 0 };
};

#define HE_DEFINE_ID(name)                                                                          \
    class name##Id : public Id                                                                      \
    {                                                                                               \
    public:                                                                                         \
        name##Id() = default;                                                                       \
                                                                                                    \
        explicit name##Id(const u64 id)                                                             \
            : Id(id)                                                                                \
        {                                                                                           \
        }                                                                                           \
                                                                                                    \
        explicit name##Id(void *ptr)                                                                \
            : Id(reinterpret_cast<u64>(ptr))                                                        \
        {                                                                                           \
        }                                                                                           \
                                                                                                    \
        bool operator==(const name##Id &other) const { return m_id == other.m_id; }                 \
        bool operator!=(const name##Id &other) const { return m_id != other.m_id; }                 \
    };                                                                                              \
                                                                                                    \
    namespace std                                                                                   \
    {                                                                                               \
        template <>                                                                                 \
        struct hash<name##Id>                                                                       \
        {                                                                                           \
            std::size_t operator()(const name##Id &id) const { return std::hash<u64>()(id.get()); } \
        };                                                                                          \
    } // namespace std
