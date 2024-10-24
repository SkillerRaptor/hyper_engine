/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_platform/window.hpp"

#include <GLFW/glfw3.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/logger.hpp>

#include "hyper_platform/key_events.hpp"
#include "hyper_platform/mouse_events.hpp"
#include "hyper_platform/window_events.hpp"

namespace hyper_platform
{
    Window::Window(const WindowDescriptor &descriptor)
        : m_native_window(nullptr)
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        m_native_window =
            glfwCreateWindow(static_cast<int>(descriptor.width), static_cast<int>(descriptor.height), descriptor.title.data(), nullptr, nullptr);
        HE_ASSERT(m_native_window);

        glfwSetWindowUserPointer(m_native_window, &descriptor.event_bus);

        glfwSetWindowSizeCallback(
            m_native_window,
            [](GLFWwindow *window, const int width, const int height)
            {
                hyper_event::EventBus &event_bus = *static_cast<hyper_event::EventBus *>(glfwGetWindowUserPointer(window));
                event_bus.dispatch(hyper_platform::WindowResizeEvent(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
            });

        glfwSetFramebufferSizeCallback(
            m_native_window,
            [](GLFWwindow *window, const int width, const int height)
            {
                hyper_event::EventBus &event_bus = *static_cast<hyper_event::EventBus *>(glfwGetWindowUserPointer(window));
                event_bus.dispatch(hyper_platform::WindowFramebufferResizeEvent(static_cast<uint32_t>(width), static_cast<uint32_t>(height)));
            });

        glfwSetWindowCloseCallback(
            m_native_window,
            [](GLFWwindow *window)
            {
                hyper_event::EventBus &event_bus = *static_cast<hyper_event::EventBus *>(glfwGetWindowUserPointer(window));
                event_bus.dispatch(hyper_platform::WindowCloseEvent());
            });

        glfwSetKeyCallback(
            m_native_window,
            [](GLFWwindow *window, const int key, const int, const int action, const int)
            {
                hyper_event::EventBus &event_bus = *static_cast<hyper_event::EventBus *>(glfwGetWindowUserPointer(window));

                switch (action)
                {
                case GLFW_PRESS:
                    event_bus.dispatch(hyper_platform::KeyPressedEvent(static_cast<KeyCode>(key)));
                    break;
                case GLFW_RELEASE:
                    event_bus.dispatch(hyper_platform::KeyReleasedEvent(static_cast<KeyCode>(key)));
                    break;
                default:
                    break;
                }
            });

        glfwSetMouseButtonCallback(
            m_native_window,
            [](GLFWwindow *window, const int button, const int action, const int)
            {
                hyper_event::EventBus &event_bus = *static_cast<hyper_event::EventBus *>(glfwGetWindowUserPointer(window));

                switch (action)
                {
                case GLFW_PRESS:
                    event_bus.dispatch(hyper_platform::MouseButtonPressedEvent(static_cast<MouseCode>(button)));
                    break;
                case GLFW_RELEASE:
                    event_bus.dispatch(hyper_platform::MouseButtonReleasedEvent(static_cast<MouseCode>(button)));
                    break;
                default:
                    break;
                }
            });

        glfwSetScrollCallback(
            m_native_window,
            [](GLFWwindow *window, const double delta_x, const double delta_y)
            {
                hyper_event::EventBus &event_bus = *static_cast<hyper_event::EventBus *>(glfwGetWindowUserPointer(window));
                event_bus.dispatch(hyper_platform::MouseScrolledEvent(static_cast<float>(delta_x), static_cast<float>(delta_y)));
            });

        glfwSetCursorPosCallback(
            m_native_window,
            [](GLFWwindow *window, const double x, const double y)
            {
                hyper_event::EventBus &event_bus = *static_cast<hyper_event::EventBus *>(glfwGetWindowUserPointer(window));
                event_bus.dispatch(hyper_platform::MouseMovedEvent(static_cast<float>(x), static_cast<float>(y)));
            });

        HE_DEBUG("Created Window with title '{}' and size {}x{}", descriptor.title, descriptor.width, descriptor.height);
    }

    Window::~Window()
    {
        glfwDestroyWindow(m_native_window);
        glfwTerminate();
    }

    uint32_t Window::width() const
    {
        int32_t width = 0;
        glfwGetFramebufferSize(m_native_window, &width, nullptr);

        return static_cast<uint32_t>(width);
    }

    uint32_t Window::height() const
    {
        int32_t height = 0;
        glfwGetFramebufferSize(m_native_window, nullptr, &height);

        return static_cast<uint32_t>(height);
    }

    GLFWwindow *Window::native_window() const
    {
        return m_native_window;
    }

    void Window::poll_events()
    {
        glfwPollEvents();
    }
} // namespace hyper_platform
