/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_platform/input.hpp>
#include <hyper_platform/window.hpp>
#include <hyper_rhi/graphics_device.hpp>

namespace he {

class Engine {
public:
    Engine();
    ~Engine() = default;

    Engine(const Engine &) = delete;
    Engine &operator=(const Engine &) = delete;

    Engine(Engine &&) noexcept = default;
    Engine &operator=(Engine &&) noexcept = default;

    void run();

private:
    void fixed_update(f32 delta_time);
    void update(f32 delta_time);
    void render() const;

private:
    OwnPtr<Window> m_window { nullptr };
    Input m_input {};

    OwnPtr<GraphicsDevice> m_graphics_device { nullptr };

    bool m_running { true };
};

} // namespace he
