/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_windowing/window.hpp"

#include <SDL3/SDL.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_windowing/event.hpp"

namespace hyper_engine
{
    Window::Window(const Logger &logger, const WindowDescriptor &descriptor)
    {
        HE_ASSERT(SDL_Init(SDL_INIT_VIDEO));

        // NOTE: Should Vulkan flag be passed on a DX12 context?
        m_native_window = SDL_CreateWindow(
            descriptor.title.data(),
            static_cast<int>(descriptor.width),
            static_cast<int>(descriptor.height),
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);
        HE_ASSERT(m_native_window);

        HE_INFO(logger, "Created Window with title '{}' and size {}x{}", descriptor.title, descriptor.width, descriptor.height);
    }

    Window::~Window()
    {
        SDL_DestroyWindow(m_native_window);
        SDL_Quit();
    }

    void Window::poll_events(const std::function<void(const Event &event)> &callback)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            // NOTE: Window Events
            case SDL_EVENT_QUIT:
                callback({
                    .type = EventType::WindowClose,
                    .window_close = WindowCloseEvent(),
                });
                break;
            case SDL_EVENT_WINDOW_MOVED:
                callback({
                    .type = EventType::WindowMove,
                    .window_move = WindowMoveEvent(event.window.data1, event.window.data2),
                });
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                callback({
                    .type = EventType::WindowResize,
                    .window_resize = WindowResizeEvent(event.window.data1, event.window.data2),
                });
                break;
            // NOTE: Key Events
            case SDL_EVENT_KEY_DOWN:
                callback({
                    .type = EventType::KeyPress,
                    .key_press = KeyPressEvent(static_cast<KeyCode>(event.key.key)),
                });
                break;
            case SDL_EVENT_KEY_UP:
                callback({
                    .type = EventType::KeyRelease,
                    .key_release = KeyReleaseEvent(static_cast<KeyCode>(event.key.key)),
                });
                break;
            // NOTE: Mouse Events
            case SDL_EVENT_MOUSE_MOTION:
                callback({
                    .type = EventType::MouseMove,
                    .mouse_move = MouseMoveEvent(event.motion.x, event.motion.y),
                });
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                callback({
                    .type = EventType::MouseButtonPress,
                    .mouse_button_press = MouseButtonPressEvent(static_cast<MouseCode>(event.button.button)),
                });
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                callback({
                    .type = EventType::MouseButtonRelease,
                    .mouse_button_release = MouseButtonReleaseEvent(static_cast<MouseCode>(event.button.button)),
                });
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                callback({
                    .type = EventType::MouseScroll,
                    .mouse_scroll = MouseScrollEvent(event.wheel.x, event.wheel.y),
                });
                break;
            default:
                break;
            }
        }
    }

    void Window::wait_events()
    {
        SDL_WaitEvent(nullptr);
    }

    uint32_t Window::width() const
    {
        int width = 0;
        SDL_GetWindowSize(m_native_window, &width, nullptr);
        return static_cast<uint32_t>(width);
    }

    uint32_t Window::height() const
    {
        int height = 0;
        SDL_GetWindowSize(m_native_window, nullptr, &height);
        return static_cast<uint32_t>(height);
    }

    SDL_Window *Window::native_window() const
    {
        return m_native_window;
    }
} // namespace hyper_engine
