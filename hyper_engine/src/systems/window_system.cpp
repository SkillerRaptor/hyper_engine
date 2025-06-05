/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "systems/window_system.hpp"

#include <SDL3/SDL.h>

#include "core/assertion.hpp"
#include "core/logger.hpp"
#include "systems/window/key_events.hpp"
#include "systems/window/mouse_events.hpp"
#include "systems/window/window_events.hpp"

WindowSystem::~WindowSystem() { SDL_Quit(); }

void WindowSystem::initialize()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        HE_PANIC("Failed to initialize SDL", SDL_GetError());
    }

    HE_INFO("Successfully initialized WindowSystem");
}

void WindowSystem::poll_events()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        // NOTE: Window Events
        case SDL_EVENT_QUIT: m_event_bus.dispatch<WindowCloseEvent>(); break;
        case SDL_EVENT_WINDOW_MOVED:
            m_event_bus.dispatch<WindowMoveEvent>(event.window.data1, event.window.data2);
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            m_event_bus.dispatch<WindowResizeEvent>(event.window.data1, event.window.data2);
            break;
        // NOTE: Key Events
        case SDL_EVENT_KEY_DOWN: m_event_bus.dispatch<KeyPressEvent>(static_cast<KeyCode>(event.key.key)); break;
        case SDL_EVENT_KEY_UP: m_event_bus.dispatch<KeyReleaseEvent>(static_cast<KeyCode>(event.key.key)); break;
        // NOTE: Mouse Events
        case SDL_EVENT_MOUSE_MOTION: m_event_bus.dispatch<MouseMoveEvent>(event.motion.x, event.motion.y); break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            m_event_bus.dispatch<MouseButtonPressEvent>(static_cast<MouseCode>(event.button.button));
            break;
        case SDL_EVENT_MOUSE_BUTTON_UP:
            m_event_bus.dispatch<MouseButtonReleaseEvent>(static_cast<MouseCode>(event.button.button));
            break;
        case SDL_EVENT_MOUSE_WHEEL: m_event_bus.dispatch<MouseScrollEvent>(event.wheel.x, event.wheel.y); break;
        default: break;
        }
    }
}

WindowId WindowSystem::create_window(const WindowDescriptor &desc)
{
    HE_ASSERT(!desc.title.empty());
    HE_ASSERT(desc.width != 0);
    HE_ASSERT(desc.height != 0);

    SDL_Window *native_window = SDL_CreateWindow(
        desc.title.c_str(), static_cast<int32_t>(desc.width), static_cast<int32_t>(desc.height),
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);
    if (native_window == nullptr)
    {
        HE_PANIC("Failed to create window", SDL_GetError());
    }

    WindowData window {};
    window.native_handle = native_window;

    return m_windows.create(window);
}

void WindowSystem::destroy_window(const WindowId id)
{
    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    SDL_DestroyWindow(window.native_handle);
    m_windows.destroy(id);
}

SDL_Window *WindowSystem::get_native_window(const WindowId id) const
{
    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    return window.native_handle;
}

void WindowSystem::set_window_title(const WindowId id, const std::string &title) const
{
    HE_ASSERT(!title.empty());
    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    SDL_SetWindowTitle(window.native_handle, title.c_str());
}

std::string WindowSystem::get_window_title(const WindowId id) const
{
    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    std::string title = SDL_GetWindowTitle(window.native_handle);
    return title;
}

void WindowSystem::set_window_size(const WindowId id, const uint32_t width, const uint32_t height) const
{
    HE_ASSERT(width > 0);
    HE_ASSERT(height > 0);

    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    SDL_SetWindowSize(window.native_handle, static_cast<int32_t>(width), static_cast<int32_t>(height));
}

glm::u32vec2 WindowSystem::get_window_size(const WindowId id) const
{
    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    int32_t width = 0;
    int32_t height = 0;
    SDL_GetWindowSize(window.native_handle, &width, &height);
    return { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };
}

void WindowSystem::set_window_fullscreen(const WindowId id, const bool fullscreen) const
{
    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    SDL_SetWindowFullscreen(window.native_handle, fullscreen);
}

bool WindowSystem::is_window_fullscreen(const WindowId id) const
{
    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    const SDL_WindowFlags window_flags = SDL_GetWindowFlags(window.native_handle);
    return (window_flags & SDL_WINDOW_FULLSCREEN) != 0;
}

void WindowSystem::set_window_resizable(const WindowId id, const bool resizable) const
{
    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    SDL_SetWindowResizable(window.native_handle, resizable);
}

bool WindowSystem::is_window_resizable(const WindowId id) const
{
    HE_ASSERT(m_windows.contains(id));

    const WindowData &window = m_windows.get(id);
    const SDL_WindowFlags window_flags = SDL_GetWindowFlags(window.native_handle);
    return (window_flags & SDL_WINDOW_RESIZABLE) != 0;
}
