/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>
#include <string>
#include <unordered_set>

#include "core/id.hpp"
#include "core/prerequisites.hpp"
#include "core/types.hpp"

HE_DEFINE_ID(Window);

class WindowServer
{
private:
    HE_NON_COPYABLE(WindowServer);
    HE_NON_MOVABLE(WindowServer);

private:
    struct Window
    {
        void *native_window { nullptr };
    };

public:
    static std::unique_ptr<WindowServer> create();
    ~WindowServer();

    WindowId create_window(std::string_view title, u32 width, u32 height);
    void destroy_window(WindowId);

    void set_title(WindowId, std::string_view);
    std::string get_title(WindowId) const;

    void set_size(WindowId, u32 width, u32 height);
    u32 get_width(WindowId) const;
    u32 get_height(WindowId) const;

    void set_fullscreen(WindowId, bool);
    bool is_fullscreen(WindowId) const;

    void set_resizable(WindowId, bool);
    bool is_resizable(WindowId) const;

    void *get_native(WindowId) const;

private:
    WindowServer() = default;

private:
    std::unordered_set<WindowId> m_windows {};
};
