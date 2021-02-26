#include "HyperSystem/HyperLinux/LinuxWindow.hpp"

#ifdef HP_PLATFORM_LINUX

#include <GLFW/glfw3.h>

#include "HyperEvent/KeyEvents.hpp"
#include "HyperEvent/MouseEvents.hpp"
#include "HyperEvent/WindowEvents.hpp"

namespace HyperSystem
{
	LinuxWindow::LinuxWindow(const HyperRendering::WindowPropsInfo& windowPropsInfo)
		: HyperRendering::Window{ windowPropsInfo }
	{
		Init();
	}

	LinuxWindow::~LinuxWindow()
	{
		Shutdown();
	}

	void LinuxWindow::Init()
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

				data.EventManager->CallEvent<HyperEvent::WindowResizeEvent>(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
				data.EventManager->CallEvent<HyperEvent::WindowCloseEvent>();
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y)
			{
				HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
				data.XPos = x;
				data.YPos = y;

				data.EventManager->CallEvent<HyperEvent::WindowMovedEvent>(static_cast<unsigned int>(x), static_cast<unsigned int>(y));
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				switch (action)
				{
				case GLFW_PRESS:
				{
					HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
					data.EventManager->CallEvent<HyperEvent::KeyPressedEvent>(key, 0);
					break;
				}
				case GLFW_RELEASE:
				{
					HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
					data.EventManager->CallEvent<HyperEvent::KeyReleasedEvent>(key);
					break;
				}
				case GLFW_REPEAT:
				{
					HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
					data.EventManager->CallEvent<HyperEvent::KeyPressedEvent>(key, 1);
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
					data.EventManager->CallEvent<HyperEvent::MouseButtonPressedEvent>(button);
					break;
				}
				case GLFW_RELEASE:
				{
					HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
					data.EventManager->CallEvent<HyperEvent::MouseButtonReleasedEvent>(button);
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
				data.EventManager->CallEvent<HyperEvent::MouseScrolledEvent>((float)xOffset, (float)yOffset);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				HyperRendering::WindowDataInfo& data = *static_cast<HyperRendering::WindowDataInfo*>(glfwGetWindowUserPointer(window));
				data.EventManager->CallEvent<HyperEvent::MouseMovedEvent>((float)xPos, (float)yPos);
			});
	}

	void LinuxWindow::Shutdown()
	{
		m_RenderContext->Shutdown();
		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void LinuxWindow::OnPreUpdate()
	{
		m_RenderContext->OnPreUpdate();
	}

	void LinuxWindow::OnUpdate(HyperUtilities::Timestep timeStep)
	{
		glfwPollEvents();
		m_RenderContext->OnUpdate(timeStep);
	}

	void LinuxWindow::OnRender()
	{
		m_RenderContext->OnRender();
	}

	void LinuxWindow::SetTitle(const std::string& title)
	{
		m_WindowDataInfo.Title = title;
		glfwSetWindowTitle(m_Window, m_WindowDataInfo.Title.c_str());
	}

	const std::string& LinuxWindow::GetTitle() const
	{
		return m_WindowDataInfo.Title;
	}

	void LinuxWindow::SetWidth(uint32_t width)
	{
		m_WindowDataInfo.Width = width;
		glfwSetWindowSize(m_Window, m_WindowDataInfo.Width, m_WindowDataInfo.Height);
	}

	uint32_t LinuxWindow::GetWidth() const
	{
		return m_WindowDataInfo.Width;
	}

	void LinuxWindow::SetHeight(uint32_t height)
	{
		m_WindowDataInfo.Height = height;
		glfwSetWindowSize(m_Window, m_WindowDataInfo.Width, m_WindowDataInfo.Height);
	}

	uint32_t LinuxWindow::GetHeight() const
	{
		return m_WindowDataInfo.Height;
	}

	void LinuxWindow::SetXPos(uint32_t xPos)
	{
		m_WindowDataInfo.XPos = xPos;
		glfwSetWindowPos(m_Window, m_WindowDataInfo.XPos, m_WindowDataInfo.YPos);
	}

	uint32_t LinuxWindow::GetXPos() const
	{
		return m_WindowDataInfo.XPos;
	}

	void LinuxWindow::SetYPos(uint32_t yPos)
	{
		m_WindowDataInfo.YPos = yPos;
		glfwSetWindowPos(m_Window, m_WindowDataInfo.XPos, m_WindowDataInfo.YPos);
	}

	uint32_t LinuxWindow::GetYPos() const
	{
		return m_WindowDataInfo.YPos;
	}

	void LinuxWindow::SetVSync(bool vSync)
	{
		m_WindowDataInfo.VSync = vSync;
		m_WindowDataInfo.VSync ? glfwSwapInterval(1) : glfwSwapInterval(0);
	}

	bool LinuxWindow::IsVSync() const
	{
		return m_WindowDataInfo.VSync;
	}

	void LinuxWindow::SetAppIcon(const std::string& imagePath)
	{
		GLFWimage icon[1] = {};
		int32_t channels = 0;
		icon[0].pixels = m_RenderContext->GetTextureManager()->LoadImage(imagePath, icon[0].width, icon[0].height, channels);
		glfwSetWindowIcon(m_Window, 1, icon);
		m_RenderContext->GetTextureManager()->FreeImage(icon[0].pixels);
	}

	double LinuxWindow::GetTime()
	{
		return glfwGetTime();
	}
}

#endif
