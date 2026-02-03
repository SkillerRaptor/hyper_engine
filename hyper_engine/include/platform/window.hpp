/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

#include <hyper_core/logger.hpp>
#include <hyper_core/prerequisites.hpp>
#include <hyper_core/types.hpp>

struct SDL_Window;

namespace he
{
    class Window
    {
    public:
        static OwnPtr<Window> create(std::string_view title, u32 width, u32 height);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        Window(Window &&) noexcept;
        Window &operator=(Window &&) noexcept;

        void update();

        void set_title(std::string_view);
        std::string title() const;

        void set_size(u32 width, u32 height);
        std::pair<u32, u32> size() const;

        void set_fullscreen(bool);
        bool is_fullscreen() const;

        void set_resizable(bool);
        bool is_resizable() const;

        HE_ALWAYS_INLINE bool is_close_requested() const { return m_close_requested; }

        HE_ALWAYS_INLINE SDL_Window *native_handle() const { return m_native_handle; }

    private:
        Window() = default;

        bool initialize(std::string_view title, u32 width, u32 height);

    private:
        SDL_Window *m_native_handle = nullptr;

        bool m_close_requested = false;
    };
} // namespace he
