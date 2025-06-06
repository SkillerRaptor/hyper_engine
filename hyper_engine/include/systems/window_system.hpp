/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <glm/vec2.hpp>

#include "core/resource_owner.hpp"
#include "systems/window/event_bus.hpp"

HE_DEFINE_ID(Window);

struct WindowDescriptor
{
    std::string title {};
    uint32_t width { 1280 };
    uint32_t height { 720 };
};

struct SDL_Window;

class WindowSystem
{
private:
    struct WindowData
    {
        SDL_Window *native_handle { nullptr };
    };

public:
    ~WindowSystem();

    void initialize();

    void poll_events();

    template <typename T>
    void register_listener(const std::function<void(const T &)> &callback)
    {
        m_event_bus.subscribe<T>(callback);
    }

    WindowId create_window(const WindowDescriptor &);
    void destroy_window(WindowId);

    SDL_Window *get_native_window(WindowId) const;

    void set_window_title(WindowId, const std::string &) const;
    std::string get_window_title(WindowId) const;

    void set_window_size(WindowId, uint32_t width, uint32_t height) const;
    glm::u32vec2 get_window_size(WindowId) const;

    void set_window_fullscreen(WindowId, bool) const;
    bool is_window_fullscreen(WindowId) const;

    void set_window_resizable(WindowId, bool) const;
    bool is_window_resizable(WindowId) const;

private:
    ResourceOwner<WindowId, WindowData> m_windows {};
    EventBus m_event_bus {};
};
