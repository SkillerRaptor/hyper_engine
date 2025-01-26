/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "camera.hpp"
#include "hyper_engine/engine.hpp"

#include <hyper_windowing/forward.hpp>

namespace hyper_engine
{
    class EditorEngine final : public Engine
    {
    public:
        bool initialize() override;
        void shutdown() override;

        void fixed_update(float delta_time, float total_time) override;
        void update(float delta_time, float total_time) override;
        void render() override;

        const Camera &camera() const override;

    private:
        Camera m_camera = Camera(glm::vec3(0.0f, 2.0f, 0.0f), -90.0f, 0.0f);
    };
} // namespace hyper_engine