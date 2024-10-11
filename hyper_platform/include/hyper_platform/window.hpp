/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string_view>

#include <hyper_event/event_bus.hpp>

struct GLFWwindow;

namespace hyper_platform
{
    struct WindowDescriptor
    {
        std::string_view title;
        uint32_t width;
        uint32_t height;
        hyper_event::EventBus &event_bus;
    };

    class Window
    {
    public:
        explicit Window(const WindowDescriptor &descriptor);
        ~Window();

        uint32_t width() const;
        uint32_t height() const;
        GLFWwindow *native_window() const;

        static void poll_events();

    private:
        GLFWwindow *m_native_window;
    };
} // namespace hyper_platform
