/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/window.hpp"

#include <GLFW/glfw3.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

namespace hyper_platform
{
    Window::Window(const WindowDescriptor &descriptor)
        : m_window(nullptr)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_window =
            glfwCreateWindow(static_cast<int>(descriptor.width), static_cast<int>(descriptor.height), descriptor.title.data(), nullptr, nullptr);
        HE_ASSERT(m_window);

        HE_DEBUG("Window created | title=\"{}\" width={} height={}", descriptor.title, descriptor.width, descriptor.height);
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
} // namespace hyper_platform
