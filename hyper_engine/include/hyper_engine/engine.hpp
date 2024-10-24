/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <chrono>
#include <memory>

#include <hyper_event/event_bus.hpp>
#include <hyper_platform/window_events.hpp>
#include <hyper_platform/window.hpp>
#include <hyper_rhi/graphics_device.hpp>
#include <hyper_render/renderer.hpp>

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
        explicit Engine(const EngineDescriptor &descriptor);

        void run();

    private:
        void on_close(const hyper_platform::WindowCloseEvent &event);
        void on_resize(const hyper_platform::WindowResizeEvent &event);

    private:
        std::chrono::steady_clock::time_point m_start_time;

        bool m_running;
        hyper_event::EventBus m_event_bus;
        hyper_platform::Window m_window;
        hyper_rhi::GraphicsDeviceHandle m_graphics_device;
        hyper_rhi::SurfaceHandle m_surface;
        hyper_render::Renderer m_renderer;
    };
} // namespace hyper_engine
