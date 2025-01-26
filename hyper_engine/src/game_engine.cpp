/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "../include/game_engine.hpp"

#include <hyper_core/assertion.hpp>

namespace hyper_engine
{
    bool GameEngine::initialize()
    {
        // FIXME: Implement Game Engine
        return false;
    }

    void GameEngine::shutdown()
    {
    }

    void GameEngine::fixed_update(const float delta_time, const float total_time)
    {
        (void) delta_time;
        (void) total_time;
    }

    void GameEngine::update(const float delta_time, const float total_time)
    {
        (void) delta_time;
        (void) total_time;
    }

    void GameEngine::render()
    {
    }

    const Camera &GameEngine::camera() const
    {
        // FIXME: Query camera from scene and entities
        HE_PANIC();
    }
} // namespace hyper_engine