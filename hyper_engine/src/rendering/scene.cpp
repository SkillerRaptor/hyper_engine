/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_rendering/scene.hpp"

namespace hyper_engine
{
    entt::registry &Scene::registry()
    {
        return m_registry;
    }

    const entt::registry &Scene::registry() const
    {
        return m_registry;
    }
} // namespace hyper_engine