/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include <hyper_windowing/forward.hpp>

namespace hyper_engine
{
    class Engine;
    class GraphicsDevice;
    class JobSystem;
    class Logger;
    class Renderer;

    class EngineLoop
    {
    public:
        EngineLoop();
        ~EngineLoop();

        bool pre_initialize(int32_t argc, const char **argv);
        bool initialize();

        void run();

    private:
        bool m_editor_enabled = false;
        std::unique_ptr<Engine> m_engine;
        bool m_exit_requested = false;

        std::unique_ptr<Logger> m_logger;
        std::unique_ptr<JobSystem> m_job_system;
        std::unique_ptr<Input> m_input;
        std::unique_ptr<Window> m_window;
        std::unique_ptr<GraphicsDevice> m_graphics_device;
        std::unique_ptr<Renderer> m_renderer;
    };
} // namespace hyper_engine