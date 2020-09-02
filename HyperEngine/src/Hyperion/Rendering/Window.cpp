#include "Window.h"

#include "Utilities/Log.h"

#include "Events/KeyEvents.h"
#include "Events/MouseEvents.h"
#include "Events/WindowEvents.h"

namespace Hyperion
{
    Window::Window(std::string title, unsigned int width, unsigned int height, bool vSync, std::queue<std::shared_ptr<Event>>* eventBus)
    {
        InitWindow(title, width, height, vSync, eventBus);
    }

    Window::~Window()
    {
        ShutdownWindow();
    }

    void Window::InitWindow(std::string title, int width, int height, bool vSync, std::queue<std::shared_ptr<Event>>* eventBus)
    {
        m_Data.Title = title;
        m_Data.Width = width;
        m_Data.Height = height;
        m_Data.VSync = vSync;
        m_Data.EventBus = eventBus;

        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        m_Window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

        if (m_Window == nullptr)
        {
            HP_CORE_FATAL("Failed to create GLFW window");
            glfwTerminate();
            std::exit(-1);
        }

        glfwMakeContextCurrent(m_Window);
        glfwSetWindowUserPointer(m_Window, &m_Data);
        glfwGetWindowPos(m_Window, (int*)&m_Data.XPos, (int*)&m_Data.YPos);
        SetVSync(false);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            HP_CORE_FATAL("Failed to initialize GLAD");
            std::exit(-1);
        }

        HP_CORE_INFO("");
        HP_CORE_INFO("OpenGL Info:");
        HP_CORE_INFO("  Vendor: %", glGetString(GL_VENDOR));
        HP_CORE_INFO("  Renderer: %", glGetString(GL_RENDERER));
        HP_CORE_INFO("  Version: %", glGetString(GL_VERSION));
        HP_CORE_INFO("");

        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.Width = width;
                data.Height = height;

                glViewport(0, 0, width, height);
                data.EventBus->push(std::make_shared<WindowResizeEvent>(width, height));
            });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.EventBus->push(std::make_shared<WindowCloseEvent>());
            });

        glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.XPos = x;
                data.YPos = y;

                data.EventBus->push(std::make_shared<WindowMovedEvent>(x, y));
            });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
            {
                switch (action)
                {
                case GLFW_PRESS:
                {
                    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                    data.EventBus->push(std::make_shared<KeyPressedEvent>(key, 0));
                    break;
                }
                case GLFW_RELEASE:
                {
                    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                    data.EventBus->push(std::make_shared<KeyReleasedEvent>(key));
                    break;
                }
                case GLFW_REPEAT:
                {
                    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                    data.EventBus->push(std::make_shared<KeyPressedEvent>(key, 1));
                    break;
                }
                }
            });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
            {
                switch (action)
                {
                case GLFW_PRESS:
                {
                    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                    data.EventBus->push(std::make_shared<MouseButtonPressedEvent>(button));
                    break;
                }
                case GLFW_RELEASE:
                {
                    WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                    data.EventBus->push(std::make_shared<MouseButtonReleasedEvent>(button));
                    break;
                }
                }
            });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.EventBus->push(std::make_shared<MouseScrolledEvent>((float)xOffset, (float)yOffset));
            });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
            {
                WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
                data.EventBus->push(std::make_shared<MouseMovedEvent>((float)xPos, (float)yPos));
            });
    }

    void Window::ShutdownWindow()
    {
        glfwDestroyWindow(m_Window);
        glfwTerminate();
    }

    void Window::OnUpdate()
    {
        glfwSwapBuffers(m_Window);
        glfwPollEvents();
    }

    void Window::SetTitle(std::string title)
    {
        m_Data.Title = title;
        glfwSetWindowTitle(m_Window, title.c_str());
    }

    std::string Window::GetTitle() const
    {
        return m_Data.Title;
    }

    void Window::SetWidth(int width)
    {
        m_Data.Width = width;
        glfwSetWindowSize(m_Window, m_Data.Width, m_Data.Height);
    }

    int Window::GetWidth() const
    {
        return m_Data.Width;
    }

    void Window::SetHeight(int height)
    {
        m_Data.Height = height;
        glfwSetWindowSize(m_Window, m_Data.Width, m_Data.Height);
    }

    int Window::GetHeight() const
    {
        return m_Data.Height;
    }

    void Window::SetXPos(int xPos)
    {
        m_Data.XPos = xPos;
        glfwSetWindowPos(m_Window, m_Data.XPos, m_Data.YPos);
    }

    int Window::GetXPos() const
    {
        return m_Data.XPos;
    }

    void Window::SetYPos(int yPos)
    {
        m_Data.YPos = yPos;
        glfwSetWindowPos(m_Window, m_Data.XPos, m_Data.YPos);
    }

    int Window::GetYPos() const
    {
        return m_Data.YPos;
    }

    void Window::SetVSync(bool vSync)
    {
        m_Data.VSync = vSync;
        m_Data.VSync ? glfwSwapInterval(1) : glfwSwapInterval(0);
    }

    bool Window::IsVSync() const
    {
        return m_Data.VSync;
    }

    GLFWwindow* Window::GetNativeWindow() const
    {
        return m_Window;
    }

    const WindowData& Window::GetWindowData() const
    {
        return m_Data;
    }
}