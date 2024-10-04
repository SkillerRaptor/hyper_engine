/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/engine.hpp"

#include <chrono>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>
#include <hyper_core/prerequisites.hpp>

namespace hyper_engine
{
    Engine::Engine(const EngineDescriptor &descriptor)
        : m_running(false)
        , m_window({
              .title = "HyperEngine",
              .width = descriptor.width,
              .height = descriptor.height,
              .event_bus = m_event_bus,
          })
        , m_graphics_device(hyper_rhi::GraphicsDevice::create({
              .graphics_api = descriptor.graphics_api,
              .debug_mode = descriptor.debug,
          }))
    {
        HE_ASSERT(m_graphics_device);

        m_event_bus.subscribe<hyper_platform::WindowCloseEvent>(HE_BIND_FUNCTION(Engine::on_close));
        m_event_bus.subscribe<hyper_platform::WindowResizeEvent>(HE_BIND_FUNCTION(Engine::on_resize));

        m_running = true;
    }

    void Engine::run()
    {
        // float time = 0.0;
        constexpr auto delta_time = static_cast<float>(1.0 / 60.0);

        std::chrono::time_point current_time = std::chrono::steady_clock::now();
        float accumulator = 0.0;

        while (m_running)
        {
            const std::chrono::time_point new_time = std::chrono::steady_clock::now();
            const float frame_time = std::chrono::duration<float>(new_time - current_time).count();
            current_time = new_time;

            accumulator += frame_time;

            hyper_platform::Window::poll_events();

            while (accumulator >= delta_time)
            {
                // Fixed Update

                accumulator -= delta_time;
                // time += delta_time;
            }

            // Update

            // Render
        }
    }

    void Engine::on_close(const hyper_platform::WindowCloseEvent &)
    {
        m_running = false;
    }

    void Engine::on_resize(const hyper_platform::WindowResizeEvent &event)
    {
        HE_DEBUG("{}, {}", event.width(), event.height());
    }
} // namespace hyper_engine
