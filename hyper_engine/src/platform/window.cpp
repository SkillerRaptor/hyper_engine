/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "platform/window.hpp"

#include <SDL3/SDL.h>

#include "core/assertion.hpp"

namespace he
{
    std::optional<Window> Window::create(const std::string_view title, const u32 width, const u32 height)
    {
        HE_ASSERT(!title.empty());
        HE_ASSERT(width > 0);
        HE_ASSERT(height > 0);

        if (SDL_WasInit(SDL_INIT_VIDEO) == 0 && !SDL_Init(SDL_INIT_VIDEO))
        {
            HE_ERROR("Failed to initialize SDL: '{}'", SDL_GetError());
            return std::nullopt;
        }

        Window window;
        if (!window.initialize(title, width, height))
        {
            HE_ERROR("Failed to initialize window");
            return std::nullopt;
        }

        return window;
    }

    bool Window::initialize(const std::string_view title, const u32 width, const u32 height)
    {
        m_native_handle = SDL_CreateWindow(
            title.data(),
            static_cast<i32>(width),
            static_cast<i32>(height),
            SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY | SDL_WINDOW_VULKAN);
        if (m_native_handle == nullptr)
        {
            HE_ERROR("Failed to create SDL window: '{}'", SDL_GetError());
            return false;
        }

        return true;
    }

    Window::~Window()
    {
        if (m_native_handle)
        {
            SDL_DestroyWindow(m_native_handle);
            SDL_Quit();
        }
    }

    Window::Window(Window &&other) noexcept
        : m_native_handle(std::exchange(other.m_native_handle, nullptr))
    {
    }

    Window &Window::operator=(Window &&other) noexcept
    {
        if (this != &other)
        {
            m_native_handle = std::exchange(other.m_native_handle, nullptr);
        }

        return *this;
    }

    void Window::update()
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                if (event.window.windowID == SDL_GetWindowID(m_native_handle))
                {
                    m_close_requested = true;
                }
                break;
            default:
                break;
            }
        }
    }

    void Window::set_title(const std::string_view title)
    {
        HE_ASSERT(!title.empty());

        SDL_SetWindowTitle(m_native_handle, title.data());
    }

    std::string Window::title() const { return SDL_GetWindowTitle(m_native_handle); }

    void Window::set_size(const u32 width, const u32 height)
    {
        HE_ASSERT(width > 0);
        HE_ASSERT(height > 0);

        SDL_SetWindowSize(m_native_handle, static_cast<i32>(width), static_cast<i32>(height));
    }

    std::pair<u32, u32> Window::size() const
    {
        i32 width = 0;
        i32 height = 0;
        SDL_GetWindowSize(m_native_handle, &width, &height);
        return { static_cast<u32>(width), static_cast<u32>(height) };
    }

    void Window::set_fullscreen(const bool fullscreen) { SDL_SetWindowFullscreen(m_native_handle, fullscreen); }

    bool Window::is_fullscreen() const
    {
        const SDL_WindowFlags flags = SDL_GetWindowFlags(m_native_handle);
        return (flags & SDL_WINDOW_FULLSCREEN) == SDL_WINDOW_FULLSCREEN;
    }

    void Window::set_resizable(const bool resizable) { SDL_SetWindowResizable(m_native_handle, resizable); }

    bool Window::is_resizable() const
    {
        const SDL_WindowFlags flags = SDL_GetWindowFlags(m_native_handle);
        return (flags & SDL_WINDOW_RESIZABLE) == SDL_WINDOW_RESIZABLE;
    }
} // namespace he
