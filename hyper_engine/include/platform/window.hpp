/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

#include "core/logger.hpp"
#include "core/prerequisites.hpp"
#include "core/types.hpp"

struct SDL_Window;

namespace he
{
    class Window
    {
    public:
        static Window create(std::string_view title, u32 width, u32 height);
        ~Window();

        Window(const Window &) = delete;
        Window &operator=(const Window &) = delete;

        Window(Window &&) noexcept;
        Window &operator=(Window &&) noexcept;

        void set_title(std::string_view);
        std::string title() const;

        void set_size(u32 width, u32 height);
        std::pair<u32, u32> size() const;

        void set_fullscreen(bool);
        bool fullscreen() const;

        void set_resizable(bool);
        bool resizable() const;

        HE_ALWAYS_INLINE SDL_Window *native_handle() const { return m_native_handle; }

    private:
        explicit Window(SDL_Window *native_handle)
            : m_native_handle { native_handle }
        {
        }

    private:
        SDL_Window *m_native_handle { nullptr };
    };
} // namespace he
