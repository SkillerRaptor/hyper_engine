#include "OpenGLWindow.hpp"

#include "HyperEvents/KeyEvents.hpp"
#include "HyperEvents/MouseEvents.hpp"
#include "HyperEvents/WindowEvents.hpp"
#include "HyperUtilities/Log.hpp"

namespace Hyperion
{
	OpenGLWindow::OpenGLWindow(const WindowData& windowData)
		: Window(windowData)
	{
		InitWindow();
	}

	OpenGLWindow::~OpenGLWindow()
	{
		ShutdownWindow();
	}

	void OpenGLWindow::InitWindow()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		m_Window = glfwCreateWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str(), nullptr, nullptr);

		if (m_Window == nullptr)
		{
			HP_CORE_FATAL("Failed to create GLFW window");
			glfwTerminate();
			std::exit(-1);
		}

		HP_CORE_DEBUG("Initializing OpenGL Window");

		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		glfwGetWindowPos(m_Window, (int*)&m_Data.XPos, (int*)&m_Data.YPos);
		SetVSync(false);

		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			HP_CORE_FATAL("Failed to initialize GLAD");
			std::exit(-1);
		}

		glViewport(0, 0, m_Data.Width, m_Data.Height);

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
				if(data.EventBus) data.EventBus->push(std::make_shared<WindowResizeEvent>(width, height));
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				if(data.EventBus) data.EventBus->push(std::make_shared<WindowCloseEvent>());
			});

		glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int x, int y)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.XPos = x;
				data.YPos = y;

				if(data.EventBus) data.EventBus->push(std::make_shared<WindowMovedEvent>(x, y));
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				switch (action)
				{
				case GLFW_PRESS:
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					if(data.EventBus) data.EventBus->push(std::make_shared<KeyPressedEvent>(key, 0));
					break;
				}
				case GLFW_RELEASE:
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					if(data.EventBus) data.EventBus->push(std::make_shared<KeyReleasedEvent>(key));
					break;
				}
				case GLFW_REPEAT:
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					if(data.EventBus) data.EventBus->push(std::make_shared<KeyPressedEvent>(key, 1));
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
					if(data.EventBus) data.EventBus->push(std::make_shared<MouseButtonPressedEvent>(button));
					break;
				}
				case GLFW_RELEASE:
				{
					WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
					if(data.EventBus) data.EventBus->push(std::make_shared<MouseButtonReleasedEvent>(button));
					break;
				}
				}
			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				if(data.EventBus) data.EventBus->push(std::make_shared<MouseScrolledEvent>((float)xOffset, (float)yOffset));
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				if(data.EventBus) data.EventBus->push(std::make_shared<MouseMovedEvent>((float)xPos, (float)yPos));
			});

		m_Context = new OpenGLContext();
	}

	void OpenGLWindow::ShutdownWindow()
	{
		HP_CORE_DEBUG("Shutdown OpenGL Window");
		delete m_Context;

		glfwDestroyWindow(m_Window);
		glfwTerminate();
	}

	void OpenGLWindow::OnTick(int currentTick)
	{
	}

	void OpenGLWindow::OnUpdate(Timestep timeStep)
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();
	}

	void OpenGLWindow::OnRender()
	{
	}

	void OpenGLWindow::SetTitle(std::string title)
	{
		m_Data.Title = title;
		glfwSetWindowTitle(m_Window, title.c_str());
	}

	std::string OpenGLWindow::GetTitle() const
	{
		return m_Data.Title;
	}

	void OpenGLWindow::SetWidth(uint32_t width)
	{
		m_Data.Width = width;
		glfwSetWindowSize(m_Window, m_Data.Width, m_Data.Height);
	}

	uint32_t OpenGLWindow::GetWidth() const
	{
		return m_Data.Width;
	}

	void OpenGLWindow::SetHeight(uint32_t height)
	{
		m_Data.Height = height;
		glfwSetWindowSize(m_Window, m_Data.Width, m_Data.Height);
	}

	uint32_t OpenGLWindow::GetHeight() const
	{
		return m_Data.Height;
	}

	void OpenGLWindow::SetXPos(uint32_t xPos)
	{
		m_Data.XPos = xPos;
		glfwSetWindowPos(m_Window, m_Data.XPos, m_Data.YPos);
	}

	uint32_t OpenGLWindow::GetXPos() const
	{
		return m_Data.XPos;
	}

	void OpenGLWindow::SetYPos(uint32_t yPos)
	{
		m_Data.YPos = yPos;
		glfwSetWindowPos(m_Window, m_Data.XPos, m_Data.YPos);
	}

	uint32_t OpenGLWindow::GetYPos() const
	{
		return m_Data.YPos;
	}

	void OpenGLWindow::SetVSync(bool vSync)
	{
		m_Data.VSync = vSync;
		m_Data.VSync ? glfwSwapInterval(1) : glfwSwapInterval(0);
	}

	bool OpenGLWindow::IsVSync() const
	{
		return m_Data.VSync;
	}

	void* OpenGLWindow::GetWindow() const
	{
		return m_Window;
	}

	RenderContext* OpenGLWindow::GetContext() const
	{
		return m_Context;
	}

	const WindowData& OpenGLWindow::GetWindowData() const
	{
		return m_Data;
	}

	Ref<Window> Window::Construct(const WindowData& windowData)
	{
		HP_CORE_DEBUG("Creating OpenGL Window");
		return CreateRef<OpenGLWindow>(windowData);
	}
}
