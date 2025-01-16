/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/own_ptr.hpp>
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
        OwnPtr<Engine> m_engine;
        bool m_exit_requested = false;

        OwnPtr<Logger> m_logger;
        OwnPtr<JobSystem> m_job_system;
        OwnPtr<Input> m_input;
        OwnPtr<Window> m_window;
        OwnPtr<GraphicsDevice> m_graphics_device;
        OwnPtr<Renderer> m_renderer;
    };
} // namespace hyper_engine