/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <concepts>
#include <unordered_map>

#include "core/assertion.hpp"
#include "core/resource_id.hpp"

template <typename Id, typename T>
    requires std::derived_from<Id, ResourceId>
class ResourceOwner
{
public:
    Id create(const T &resource)
    {
        // FIXME: Replace this with placement new, when adding custom memory pool
        const Id id = create_id();
        m_resources.insert({ id.get(), resource });

        return id;
    }

    void destroy(const Id id)
    {
        HE_ASSERT(contains(id));

        m_resources.erase(id.get());
        destroy_id(id);
    }

    T &get(const Id id)
    {
        HE_ASSERT(contains(id));

        return m_resources[id.get()];
    }

    const T &get(const Id id) const
    {
        HE_ASSERT(contains(id));

        return m_resources.at(id.get());
    }

    bool contains(const Id id) const
    {
        const uint64_t internal_id = id.id();
        if (m_ids.size() < internal_id)
        {
            return false;
        }

        return m_ids[internal_id].get() == id.get();
    }

private:
    Id create_id()
    {
        // 1. Check if there is any unrecyclable id available
        if (m_unrecycled_ids == 0)
        {
            // 2. Create new id based on the size of already created ids
            const uint32_t new_id = static_cast<uint32_t>(m_ids.size());
            const Id id { new_id, 0 };
            m_ids.push_back(id);

            return id;
        }

        // 2. Retrieve the `Id` in the vector at `m_next_unrecycled_id`
        const uint32_t current_unrecycled_id = m_next_unrecycled_id;
        const Id unrecycled_id = m_ids.at(current_unrecycled_id);

        // 3. Swap the internal id of `m_next_unrecycled_id` and the `Id` pointed by `m_next_unrecycled_id`
        m_ids[current_unrecycled_id] = { current_unrecycled_id, unrecycled_id.version() };
        m_next_unrecycled_id = unrecycled_id.id();

        // 4. Decrease the amount of unrecycled ids
        m_unrecycled_ids -= 1;

        return m_ids[current_unrecycled_id];
    }

    void destroy_id(const Id id)
    {
        // 1. Retrieve the `Id` in the vector at the internal id of `p_id`
        const uint32_t internal_id = id.id();
        const Id real_id = m_ids.at(internal_id);

        // 2. Swap the internal id of `m_next_unrecycled_id` and the retrieved `Id`
        const uint32_t next_unrecycled_id = m_next_unrecycled_id;
        m_ids[internal_id] = { next_unrecycled_id, real_id.version() + 1 };
        m_next_unrecycled_id = real_id.id();

        // 3. Increase the amount of unrecycled ids
        m_unrecycled_ids += 1;
    }

private:
    size_t m_unrecycled_ids { 0 };
    uint32_t m_next_unrecycled_id { std::numeric_limits<uint32_t>::max() };
    std::vector<Id> m_ids {};

    std::unordered_map<uint64_t, T> m_resources {};
};
