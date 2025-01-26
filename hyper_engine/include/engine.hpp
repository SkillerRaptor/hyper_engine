/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_rendering/scene.hpp>

namespace hyper_engine
{
    class Camera;

    class Engine
    {
    public:
        virtual ~Engine() = default;

        virtual bool initialize() = 0;
        virtual void shutdown() = 0;

        virtual void fixed_update(float delta_time, float total_time) = 0;
        virtual void update(float delta_time, float total_time) = 0;
        virtual void render() = 0;

        // FIXME: This should be done better somehow
        virtual const Camera &camera() const = 0;

        const Scene &scene() const;

    protected:
        Scene m_scene;
    };
} // namespace hyper_engine