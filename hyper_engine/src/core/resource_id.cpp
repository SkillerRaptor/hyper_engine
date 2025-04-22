/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-ResourceIdentifier: MIT
 */

#include "core/resource_id.hpp"

ResourceId::ResourceId(const uint32_t id, const uint32_t version)
    : m_id(((static_cast<uint64_t>(id) << s_id_shift) & s_id_mask) | (static_cast<uint64_t>(version) & s_version_mask))
{
}

uint32_t ResourceId::id() const
{
    return static_cast<uint32_t>((m_id & s_id_mask) >> s_id_shift);
}

uint32_t ResourceId::version() const
{
    return static_cast<uint32_t>(m_id & s_version_mask);
}

uint64_t ResourceId::get() const
{
    return m_id;
}