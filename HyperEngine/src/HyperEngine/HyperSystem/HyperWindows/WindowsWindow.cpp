#include "HyperSystem/HyperWindows/WindowsWindow.hpp"

#include <GLFW/glfw3.h>

#include "HyperEvent/KeyEvents.hpp"
#include "HyperEvent/MouseEvents.hpp"
#include "HyperEvent/WindowEvents.hpp"

namespace HyperSystem
{
	WindowsWindow::WindowsWindow(const HyperRendering::WindowPropsInfo& windowPropsInfo)
		: HyperRendering::Window{ windowPropsInfo }
	{
		Init();
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::Init()
	{
		m_RenderContext = HyperRendering::RenderContext::Construct(m_GraphicsAPI, m_WindowDataInfo.Title);

		glfwInit();
		m_RenderContext->PreInit();
		m_Window = glfwCreateWindow(m_WindowDataInfo.Width, m_WindowDataInfo.Height, m_WindowDataInfo.Title.c_str(), nullptr, nullptr);

		if (m_Window == nullptr)
		{
			HP_ASSERT(false, "Failed to create GLFW window");
			glfwTerminate();
			std::exit(EXIT_FAILURE);
		}

		m_RenderContext->SetWindow(m_Window);
		m_RenderContext->Init();

		m_WindowDataInfo.Context = m_RenderContext;

		glfwSetWindowUserPointer(m_Window, &m_WindowDataInfo);
		glfwGetWindowPos(m_Window, (int*)&m_WindowDataInfo.XPos, (int*)&m_WindowDataInfo.YPos);
		SetVSync(m_WindowDataInfo.VSync);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
				data.Width = width;
				data.Height = height;

				data.Context->OnResize(static_cast<size_t>(width), static_cast<size_t>(height));

				if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::WindowResizeEvent>(width, height));
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
				if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::WindowCloseEvent>());
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y)
			{
				HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
				data.XPos = x;
				data.YPos = y;

				if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::WindowMovedEvent>(x, y));
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				switch (action)
				{
				case GLFW_PRESS:
				{
					HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::KeyPressedEvent>(key, 0));
					break;
				}
				case GLFW_RELEASE:
				{
					HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::KeyReleasedEvent>(key));
					break;
				}
				case GLFW_REPEAT:
				{
					HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::KeyPressedEvent>(key, 1));
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
					HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::MouseButtonPressedEvent>(button));
					break;
				}
				case GLFW_RELEASE:
				{
					HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
					if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::MouseButtonReleasedEvent>(button));
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
				if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::MouseScrolledEvent>((float)xOffset, (float)yOffset));
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
				if (data.EventBus) data.EventBus->push(std::make_shared<HyperEvent::MouseMovedEvent>((float)xPos, (float)yPos));
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

	void WindowsWindow::OnUpdate(HyperUtilities::Timestep timeStep)
	{
		glfwPollEvents();
		m_RenderContext->OnUpdate(timeStep);
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

	void WindowsWindow::SetAppIcon(const std::string& imagePath)
	{
		GLFWimage icon[1] = {};
		int32_t channels = 0;
		icon[0].pixels = m_RenderContext->GetTextureManager()->LoadImage(imagePath, icon[0].width, icon[0].height, channels);
		glfwSetWindowIcon(m_Window, 1, icon);
		m_RenderContext->GetTextureManager()->FreeImage(icon[0].pixels);
	}

	double WindowsWindow::GetTime()
	{
		return glfwGetTime();
	}
}
