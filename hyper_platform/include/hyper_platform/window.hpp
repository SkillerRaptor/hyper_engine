/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string_view>

struct GLFWwindow;

namespace hyper_platform
{
    struct WindowDescriptor
    {
        std::string_view title;
        uint32_t width;
        uint32_t height;
    };

    class Window
    {
    public:
        Window(const WindowDescriptor &descriptor);
        ~Window();

    private:
        GLFWwindow *m_window;
    };
} // namespace hyper_platform
