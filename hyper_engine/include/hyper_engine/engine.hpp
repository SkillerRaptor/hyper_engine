/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

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
        hyper_platform::Window m_window;
    };
} // namespace hyper_engine
