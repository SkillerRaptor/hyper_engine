#include "WindowsWindow.hpp"

#include <GLFW/glfw3.h>

#include "HyperEvents/KeyEvents.hpp"
#include "HyperEvents/MouseEvents.hpp"
#include "HyperEvents/WindowEvents.hpp"

namespace Hyperion
{
	WindowsWindow::WindowsWindow(const WindowPropsInfo& windowPropsInfo)
		: Window(windowPropsInfo)
	{
		Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init()
	{
		m_RenderContext = RenderContext::Construct(m_GraphicsAPI, m_WindowDataInfo.Title);

		glfwInit();
		m_RenderContext->PreInit();
		m_Window = glfwCreateWindow(m_WindowDataInfo.Width, m_WindowDataInfo.Height, m_WindowDataInfo.Title.c_str(), nullptr, nullptr);

		if (m_Window == nullptr)
		{
			HP_CORE_FATAL("Failed to create GLFW window");
			glfwTerminate();
			std::exit(-1);
		}

		m_RenderContext->m_Window = m_Window;
		m_RenderContext->Init();

		glfwSetWindowUserPointer(m_Window, &m_WindowDataInfo);
		glfwGetWindowPos(m_Window, (int*)&m_WindowDataInfo.XPos, (int*)&m_WindowDataInfo.YPos);
		SetVSync(m_WindowDataInfo.VSync);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
				data.Width = width;
				data.Height = height;

				if (data.EventBus) data.EventBus->push(std::make_shared<WindowResizeEvent>(width, height));
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
				if (data.EventBus) data.EventBus->push(std::make_shared<WindowCloseEvent>());
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y)
			{
				WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
				data.XPos = x;
				data.YPos = y;

				if (data.EventBus) data.EventBus->push(std::make_shared<WindowMovedEvent>(x, y));
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				switch (action)
				{
				case GLFW_PRESS:
				{
					WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<KeyPressedEvent>(key, 0));
					break;
				}
				case GLFW_RELEASE:
				{
					WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<KeyReleasedEvent>(key));
					break;
				}
				case GLFW_REPEAT:
				{
					WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<KeyPressedEvent>(key, 1));
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
					WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<MouseButtonPressedEvent>(button));
					break;
				}
				case GLFW_RELEASE:
				{
					WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<MouseButtonReleasedEvent>(button));
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
				if (data.EventBus) data.EventBus->push(std::make_shared<MouseScrolledEvent>((float)xOffset, (float)yOffset));
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowDataInfo& data = *static_cast<WindowDataInfo*>(glfwGetWindowUserPointer(window));
				if (data.EventBus) data.EventBus->push(std::make_shared<MouseMovedEvent>((float)xPos, (float)yPos));
			});
	}

	void WindowsWindow::Shutdown()
	{
		m_RenderContext->Shutdown();
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void WindowsWindow::OnPreUpdate()
	{
		m_RenderContext->OnPreUpdate();
	}

	void WindowsWindow::OnUpdate(Timestep timeStep)
	{
		m_RenderContext->OnUpdate(timeStep);
		glfwPollEvents();
	}

	void WindowsWindow::OnRender()
	{
		m_RenderContext->OnRender();
	}

	void WindowsWindow::SetTitle(const std::string& title)
	{
		m_WindowDataInfo.Title = title;
		glfwSetWindowTitle(m_Window, m_WindowDataInfo.Title.c_str());
	}

	const std::string& WindowsWindow::GetTitle() const
	{
		return m_WindowDataInfo.Title;
	}

	void WindowsWindow::SetWidth(uint32_t width)
	{
		m_WindowDataInfo.Width = width;
		glfwSetWindowSize(m_Window, m_WindowDataInfo.Width, m_WindowDataInfo.Height);
	}

	uint32_t WindowsWindow::GetWidth() const
	{
		return m_WindowDataInfo.Width;
	}

	void WindowsWindow::SetHeight(uint32_t height)
	{
		m_WindowDataInfo.Height = height;
		glfwSetWindowSize(m_Window, m_WindowDataInfo.Width, m_WindowDataInfo.Height);
	}

	uint32_t WindowsWindow::GetHeight() const
	{
		return m_WindowDataInfo.Height;
	}

	void WindowsWindow::SetXPos(uint32_t xPos)
	{
		m_WindowDataInfo.XPos = xPos;
		glfwSetWindowPos(m_Window, m_WindowDataInfo.XPos, m_WindowDataInfo.YPos);
	}

	uint32_t WindowsWindow::GetXPos() const
	{
		return m_WindowDataInfo.XPos;
	}

	void WindowsWindow::SetYPos(uint32_t yPos)
	{
		m_WindowDataInfo.YPos = yPos;
		glfwSetWindowPos(m_Window, m_WindowDataInfo.XPos, m_WindowDataInfo.YPos);
	}

	uint32_t WindowsWindow::GetYPos() const
	{
		return m_WindowDataInfo.YPos;
	}

	void WindowsWindow::SetVSync(bool vSync)
	{
		m_WindowDataInfo.VSync = vSync;
		m_WindowDataInfo.VSync ? glfwSwapInterval(1) : glfwSwapInterval(0);
	}

	bool WindowsWindow::IsVSync() const
	{
		return m_WindowDataInfo.VSync;
	}

	void* WindowsWindow::GetWindow()
	{
		return m_Window;
	}
}
