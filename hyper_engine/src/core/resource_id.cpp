/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-ResourceIdentifier: MIT
 */

#include "core/resource_id.hpp"

ResourceId::ResourceId(const uint32_t id, const uint32_t version)
    : m_id { ((static_cast<uint64_t>(id) << s_id_shift) & s_id_mask)
             | (static_cast<uint64_t>(version) & s_version_mask) }
{
}
