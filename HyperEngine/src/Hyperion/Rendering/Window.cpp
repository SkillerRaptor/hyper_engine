#include "Window.h"

#include "Utilities/Log.h"

namespace Hyperion
{
    Window::Window(std::string title, unsigned int width, unsigned int height, bool vSync)
    {
        InitWindow(title, width, height, vSync);
    }

    Window::~Window()
    {
        ShutdownWindow();
    }

    void Window::InitWindow(std::string title, int width, int height, bool vSync)
    {
        m_Data.Title = title;
        m_Data.Width = width;
        m_Data.Height = height;
        m_Data.VSync = vSync;

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