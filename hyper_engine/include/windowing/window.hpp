/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>
#include <string_view>

#include <hyper_core/forward.hpp>

struct SDL_Window;

namespace hyper_engine
{
    struct Event;

    struct WindowDescriptor
    {
        std::string_view title;
        uint32_t width = 1280;
        uint32_t height = 720;
    };

    class Window
    {
    public:
        Window(const Logger &logger, const WindowDescriptor &descriptor);
        ~Window();

        static void poll_events(const std::function<void(const Event &event)> &callback);
        static void wait_events();

        uint32_t width() const;
        uint32_t height() const;
        SDL_Window *native_window() const;

    private:
        SDL_Window *m_native_window = nullptr;
    };
} // namespace hyper_engine
