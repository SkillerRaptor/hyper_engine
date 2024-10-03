/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include <hyper_event/event_bus.hpp>
#include <hyper_platform/window_events.hpp>
#include <hyper_platform/window.hpp>
#include <hyper_rhi/graphics_device.hpp>

namespace hyper_engine
{
    struct EngineDescriptor
    {
        uint32_t width;
        uint32_t height;
        hyper_rhi::GraphicsApi graphics_api;
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
        std::unique_ptr<hyper_rhi::GraphicsDevice> m_graphics_device;
    };
} // namespace hyper_engine
