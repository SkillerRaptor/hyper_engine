/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "platform/window_server.hpp"

#include <SDL3/SDL.h>

#include "core/assertion.hpp"
#include "core/logger.hpp"

std::unique_ptr<WindowServer> WindowServer::create()
{
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        HE_FATAL("Failed to initialize SDL: {}", SDL_GetError());
        return nullptr;
    }

    HE_INFO("Initialized window server");
    return std::unique_ptr<WindowServer>(new WindowServer());
}

WindowServer::~WindowServer()
{
    SDL_Quit();
    HE_INFO("Shutdown window server");
}

WindowId WindowServer::create_window(const std::string_view title, const u32 width, const u32 height)
{
    HE_ASSERT(!title.empty());
    HE_ASSERT(width > 0);
    HE_ASSERT(height > 0);

    SDL_Window *native_window = SDL_CreateWindow(title.data(), static_cast<i32>(width), static_cast<i32>(height),
        SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);
    HE_ASSERT(native_window != nullptr);

    Window *window = new Window {
        .native_window = native_window,
    };

    const WindowId id { window };
    m_windows.insert(id);
    return id;
}

void WindowServer::destroy_window(const WindowId id)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));

    const Window *window = id.as<Window>();
    SDL_DestroyWindow(static_cast<SDL_Window *>(window->native_window));
    m_windows.erase(id);
}

void WindowServer::set_title(const WindowId id, const std::string_view title)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));
    HE_ASSERT(!title.empty());

    const Window *window = id.as<Window>();
    SDL_SetWindowTitle(static_cast<SDL_Window *>(window->native_window), title.data());
}

std::string WindowServer::get_title(const WindowId id) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));

    const Window *window = id.as<Window>();
    return SDL_GetWindowTitle(static_cast<SDL_Window *>(window->native_window));
}

void WindowServer::set_size(const WindowId id, const u32 width, const u32 height)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));
    HE_ASSERT(width > 0);
    HE_ASSERT(height > 0);

    const Window *window = id.as<Window>();
    SDL_SetWindowSize(static_cast<SDL_Window *>(window->native_window), static_cast<i32>(width), static_cast<i32>(height));
}

u32 WindowServer::get_width(const WindowId id) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));

    const Window *window = id.as<Window>();
    i32 width { 0 };
    SDL_GetWindowSize(static_cast<SDL_Window *>(window->native_window), &width, nullptr);
    return static_cast<u32>(width);
}

u32 WindowServer::get_height(const WindowId id) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));

    const Window *window = id.as<Window>();
    i32 height { 0 };
    SDL_GetWindowSize(static_cast<SDL_Window *>(window->native_window), nullptr, &height);
    return static_cast<u32>(height);
}

void WindowServer::set_fullscreen(const WindowId id, const bool fullscreen)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));

    const Window *window = id.as<Window>();
    SDL_SetWindowFullscreen(static_cast<SDL_Window *>(window->native_window), fullscreen);
}

bool WindowServer::is_fullscreen(const WindowId id) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));

    const Window *window = id.as<Window>();
    const SDL_WindowFlags flags = SDL_GetWindowFlags(static_cast<SDL_Window *>(window->native_window));
    return flags & SDL_WINDOW_FULLSCREEN;
}

void WindowServer::set_resizable(const WindowId id, const bool resizable)
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));

    const Window *window = id.as<Window>();
    SDL_SetWindowResizable(static_cast<SDL_Window *>(window->native_window), resizable);
}

bool WindowServer::is_resizable(const WindowId id) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));

    const Window *window = id.as<Window>();
    const SDL_WindowFlags flags = SDL_GetWindowFlags(static_cast<SDL_Window *>(window->native_window));
    return flags & SDL_WINDOW_RESIZABLE;
}

void *WindowServer::get_native(const WindowId id) const
{
    HE_ASSERT(id.is_valid());
    HE_ASSERT(m_windows.contains(id));

    const Window *window = id.as<Window>();
    return window->native_window;
}
