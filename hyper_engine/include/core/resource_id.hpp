/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

class ResourceId
{
public:
    static constexpr uint64_t s_id_mask = 0xffffffff00000000;
    static constexpr uint64_t s_id_shift = 32;

    static constexpr uint64_t s_version_mask = ~s_id_mask;

public:
    ResourceId() = default;
    ResourceId(uint32_t id, uint32_t version);

    uint32_t id() const;
    uint32_t version() const;

    uint64_t get() const;

private:
    uint64_t m_id = 0;
};

#define HE_DEFINE_ID(name)                                           \
    class name##Id : public ResourceId                               \
    {                                                                \
    public:                                                          \
        name##Id() = default;                                        \
                                                                     \
        explicit name##Id(const uint32_t id, const uint32_t version) \
            : ResourceId(id, version)                                \
        {                                                            \
        }                                                            \
    }