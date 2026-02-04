/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "engine.hpp"

#include <chrono>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

namespace he
{
    Engine::Engine()
    {
        const std::chrono::steady_clock::time_point start_time = std::chrono::steady_clock::now();

        m_window = make_own<Window>("HyperEngine", 1280, 720);
        HE_ASSERT(m_window != nullptr);

        const Validation validation =
#if HE_DEBUG_BUILD
            Validation::Enabled;
#elif HE_RELEASE_BUILD
            Validation::Disabled;
#endif

        m_graphics_device = GraphicsDevice::create(GraphicsApi::Vulkan, *m_window, validation);
        HE_ASSERT(m_graphics_device != nullptr);

        const std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now();
        const std::chrono::duration<f64> elapsed_seconds = end_time - start_time;

        HE_INFO("Completed engine initialization (time={:.2}s)", elapsed_seconds.count());
    }

    void Engine::run()
    {
        f32 total_time = 0.0;
        constexpr f32 delta_time = 1.0f / 60.0f;

        f32 accumulator = 0.0;
        std::chrono::time_point current_time = std::chrono::steady_clock::now();
        while (!m_window->is_close_requested())
        {
            const std::chrono::time_point new_time = std::chrono::steady_clock::now();
            const f32 frame_time = std::chrono::duration<f32>(new_time - current_time).count();
            current_time = new_time;

            accumulator += frame_time;

            m_window->update();
            m_input.update();

            while (accumulator >= delta_time)
            {
                // Fixed Update
                fixed_update(delta_time);

                accumulator -= delta_time;
                total_time += delta_time;
            }

            // Update
            update(delta_time);

            // Render
            render();
        }
    }

    void Engine::fixed_update(const f32 delta_time) { (void) delta_time; }

    void Engine::update(const f32 delta_time) { (void) delta_time; }

    void Engine::render() const { }
} // namespace he
