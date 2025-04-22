/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <glm/vec2.hpp>

#include "core/resource_owner.hpp"

using WS = class WindowSystem;

struct SDL_Window;

class WindowSystem
{
public:
    HE_DEFINE_ID(Window);

    struct WindowDescriptor
    {
        std::string title;
        uint32_t width = 1280;
        uint32_t height = 720;
    };

private:
    struct WindowData
    {
        SDL_Window *native_handle;
    };

public:
    ~WindowSystem();

    void initialize();

    void poll_events();

    WindowId create_window(const WindowDescriptor &descriptor);
    void destroy_window(WindowId id);

    SDL_Window *get_native_window(WindowId id) const;

    void set_window_title(WindowId id, const std::string &title) const;
    std::string get_window_title(WindowId id) const;

    void set_window_size(WindowId id, uint32_t width, uint32_t height) const;
    glm::u32vec2 get_window_size(WindowId id) const;

    void set_window_fullscreen(WindowId id, bool fullscreen) const;
    bool is_window_fullscreen(WindowId id) const;

    void set_window_resizable(WindowId id, bool resizable) const;
    bool is_window_resizable(WindowId id) const;

private:
    ResourceOwner<WindowId, WindowData> m_windows;
};