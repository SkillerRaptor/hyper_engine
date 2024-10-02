/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_event/event_bus.hpp>
#include <hyper_platform/window_events.hpp>
#include <hyper_platform/window.hpp>

namespace hyper_engine
{
    enum GraphicsApi
    {
        D3D12,
        Vulkan,
    };

    struct EngineDescriptor
    {
        uint32_t width;
        uint32_t height;
        GraphicsApi graphics_api;
        bool debug;
    };

    class Engine
    {
    public:
        Engine(const EngineDescriptor &descriptor);

        void run();

    private:
        void on_close(const hyper_platform::WindowCloseEvent &event);
        void on_resize(const hyper_platform::WindowResizeEvent &event);

    private:
        bool m_running;
        hyper_event::EventBus m_event_bus;
        hyper_platform::Window m_window;
    };
} // namespace hyper_engine
