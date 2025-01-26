/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "engine.hpp"

namespace hyper_engine
{
    class GameEngine final : public Engine
    {
    public:
        bool initialize() override;
        void shutdown() override;

        void fixed_update(float delta_time, float total_time) override;
        void update(float delta_time, float total_time) override;
        void render() override;

        const Camera &camera() const override;
    };
} // namespace hyper_engine