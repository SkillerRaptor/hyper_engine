/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/engine.hpp"

#include <hyper_core/logger.hpp>
#include <hyper_core/prerequisites.hpp>
#include <hyper_platform/window.hpp>

namespace hyper_engine
{
    Engine::Engine(const EngineDescriptor &descriptor)
        : m_window({
              .title = "HyperEngine",
              .width = descriptor.width,
              .height = descriptor.height,
              .event_bus = m_event_bus,
          })
    {
        m_event_bus.subscribe<hyper_platform::WindowResizeEvent>(HE_BIND_FUNCTION(Engine::on_resize));
    }

    void Engine::run()
    {
        while (true)
        {
            m_window.poll_events();
        }
    }

    void Engine::on_resize(const hyper_platform::WindowResizeEvent &event)
    {
        HE_DEBUG("{}, {}", event.width(), event.height());
    }
} // namespace hyper_engine
