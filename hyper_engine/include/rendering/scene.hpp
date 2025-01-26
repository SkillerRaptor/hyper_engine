/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <entt/entt.hpp>

namespace hyper_engine
{
    class Scene
    {
    public:
        entt::registry &registry();
        const entt::registry &registry() const;

    private:
        entt::registry m_registry;
    };
} // namespace hyper_engine