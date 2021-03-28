#include "Window.hpp"

#include "HyperOpenGL33/OpenGL33Context.hpp"
#include "HyperVulkan/VulkanContext.hpp"

#include <HyperCore/HyperAssert.hpp>
#include <HyperEvent/KeyEvents.hpp>
#include <HyperEvent/MouseEvents.hpp>
#include <HyperEvent/WindowEvents.hpp>

#include <GLFW/glfw3.h>

#include <utility>

namespace HyperEngine
{
	bool Window::Initialize(WindowInfo windowInfo)
	{
		m_windowInfo = std::move(windowInfo);
		
		m_pContext = new OpenGL33Context{};
		//m_pRenderContext = new VulkanRenderContext{};
		
		glfwInit();
		m_pContext->SetWindowHints();
		m_pWindow = glfwCreateWindow(m_windowInfo.width, m_windowInfo.height, m_windowInfo.title.c_str(), nullptr, nullptr);
		if (m_pWindow == nullptr)
		{
			HYPERENGINE_ASSERT(false, "Failed to created GLFW window!");
			glfwDestroyWindow(m_pWindow);
			glfwTerminate();
			return false;
		}
		
		if (m_pContext->Initialize(m_pWindow))
		{
			HYPERENGINE_ASSERT(false, "Failed to initialize render context!");
			glfwDestroyWindow(m_pWindow);
			glfwTerminate();
			return false;
		}
		
		glfwSetWindowUserPointer(m_pWindow, &m_windowInfo);
		
		SetDecorated(m_windowInfo.isDecorated);
		SetResizable(m_windowInfo.isResizable);
		SetVsync(m_windowInfo.isVsync);
		SetShown(m_windowInfo.isShown);
		SetFocused(m_windowInfo.isFocused);
		
		glfwSetWindowPosCallback(m_pWindow, [](GLFWwindow* pWindow, int32_t x, int32_t y)
		{
			void* pWindowUserPointer{ glfwGetWindowUserPointer(pWindow) };
			WindowInfo& windowInfo{ *reinterpret_cast<WindowInfo*>(pWindowUserPointer) };
			windowInfo.position = Vec2ui{ static_cast<uint32_t>(x), static_cast<uint32_t>(y) };
			
			windowInfo.pEventManager->InvokeEvent<WindowMovedEvent>(static_cast<uint32_t>(x), static_cast<uint32_t>(y));
		});
		
		glfwSetWindowSizeCallback(m_pWindow, [](GLFWwindow* pWindow, int32_t width, int32_t height)
		{
			void* pWindowUserPointer{ glfwGetWindowUserPointer(pWindow) };
			WindowInfo& windowInfo{ *reinterpret_cast<WindowInfo*>(pWindowUserPointer) };
			windowInfo.width = static_cast<uint32_t>(width);
			windowInfo.height = static_cast<uint32_t>(height);
			
			windowInfo.pEventManager->InvokeEvent<WindowResizeEvent>(static_cast<uint32_t>(width), static_cast<uint32_t>(height));
		});
		
		glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* pWindow)
		{
			void* pWindowUserPointer{ glfwGetWindowUserPointer(pWindow) };
			WindowInfo& windowInfo{ *reinterpret_cast<WindowInfo*>(pWindowUserPointer) };
			
			windowInfo.pEventManager->InvokeEvent<WindowCloseEvent>();
		});
		
		glfwSetWindowFocusCallback(m_pWindow, [](GLFWwindow* pWindow, int32_t focused)
		{
			void* pWindowUserPointer{ glfwGetWindowUserPointer(pWindow) };
			WindowInfo& windowInfo{ *reinterpret_cast<WindowInfo*>(pWindowUserPointer) };
			windowInfo.isFocused = static_cast<bool>(focused);
			
			if (focused)
			{
				windowInfo.pEventManager->InvokeEvent<WindowFocusEvent>();
			}
			else
			{
				windowInfo.pEventManager->InvokeEvent<WindowLostFocusEvent>();
			}
		});
		
		glfwSetKeyCallback(m_pWindow, [](GLFWwindow* pWindow, int32_t key, int32_t scancode, int32_t action, int32_t mods)
		{
			void* pWindowUserPointer{ glfwGetWindowUserPointer(pWindow) };
			WindowInfo& windowInfo{ *reinterpret_cast<WindowInfo*>(pWindowUserPointer) };
			
			switch (action)
			{
			case GLFW_PRESS:
				windowInfo.pEventManager->InvokeEvent<KeyPressedEvent>(key, 0);
				break;
			case GLFW_RELEASE:
				windowInfo.pEventManager->InvokeEvent<KeyReleasedEvent>(key);
				break;
			case GLFW_REPEAT:
				windowInfo.pEventManager->InvokeEvent<KeyPressedEvent>(key, 1);
				break;
			default:
				break;
			}
		});
		
		glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* pWindow, int32_t button, int32_t action, int32_t mods)
		{
			void* pWindowUserPointer{ glfwGetWindowUserPointer(pWindow) };
			WindowInfo& windowInfo{ *reinterpret_cast<WindowInfo*>(pWindowUserPointer) };
			
			switch (action)
			{
			case GLFW_PRESS:
				windowInfo.pEventManager->InvokeEvent<MouseButtonPressedEvent>(button);
				break;
			case GLFW_RELEASE:
				windowInfo.pEventManager->InvokeEvent<MouseButtonReleasedEvent>(button);
				break;
			default:
				break;
			}
		});
		
		glfwSetScrollCallback(m_pWindow, [](GLFWwindow* pWindow, double xOffset, double yOffset)
		{
			void* pWindowUserPointer{ glfwGetWindowUserPointer(pWindow) };
			WindowInfo& windowInfo{ *reinterpret_cast<WindowInfo*>(pWindowUserPointer) };
			windowInfo.pEventManager->InvokeEvent<MouseScrolledEvent>((float) xOffset, (float) yOffset);
		});
		
		glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* pWindow, double xPos, double yPos)
		{
			void* pWindowUserPointer{ glfwGetWindowUserPointer(pWindow) };
			WindowInfo& windowInfo{ *reinterpret_cast<WindowInfo*>(pWindowUserPointer) };
			windowInfo.pEventManager->InvokeEvent<MouseMovedEvent>((float) xPos, (float) yPos);
		});
		
		return true;
	}
	
	void Window::Terminate()
	{
		m_pContext->Terminate();
		delete m_pContext;
		
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}
	
	void Window::Present()
	{
		m_pContext->Present();
	}
	
	void Window::SetTitle(const std::string& title)
	{
		m_windowInfo.title = title;
		glfwSetWindowTitle(m_pWindow, m_windowInfo.title.c_str());
	}
	
	const std::string& Window::GetTitle() const
	{
		return m_windowInfo.title;
	}
	
	void Window::SetPosition(Vec2ui position)
	{
		m_windowInfo.position = position;
		glfwSetWindowPos(m_pWindow, static_cast<int>(m_windowInfo.position.x), static_cast<int>(m_windowInfo.position.y));
	}
	
	Vec2ui Window::GetPosition() const
	{
		return m_windowInfo.position;
	}
	
	void Window::SetWidth(uint32_t width)
	{
		m_windowInfo.width = width;
		glfwSetWindowSize(m_pWindow, static_cast<int>(m_windowInfo.width), static_cast<int>(m_windowInfo.height));
	}
	
	uint32_t Window::GetWidth() const
	{
		return m_windowInfo.width;
	}
	
	void Window::SetHeight(uint32_t height)
	{
		m_windowInfo.height = height;
		glfwSetWindowSize(m_pWindow, static_cast<int>(m_windowInfo.width), static_cast<int>(m_windowInfo.height));
	}
	
	uint32_t Window::GetHeight() const
	{
		return m_windowInfo.height;
	}
	
	void Window::SetDecorated(bool isDecorated)
	{
		m_windowInfo.isDecorated = isDecorated;
		glfwSetWindowAttrib(m_pWindow, GLFW_DECORATED, m_windowInfo.isDecorated);
	}
	
	bool Window::IsDecorated() const
	{
		return m_windowInfo.isDecorated;
	}
	
	void Window::SetResizable(bool isResizable)
	{
		m_windowInfo.isResizable = isResizable;
		glfwSetWindowAttrib(m_pWindow, GLFW_RESIZABLE, m_windowInfo.isResizable);
	}
	
	bool Window::IsResizable() const
	{
		return m_windowInfo.isResizable;
	}
	
	void Window::SetVsync(bool isVsync)
	{
		m_windowInfo.isVsync = isVsync;
		m_windowInfo.isVsync ? glfwSwapInterval(1) : glfwSwapInterval(0);
	}
	
	bool Window::IsVsync() const
	{
		return m_windowInfo.isVsync;
	}
	
	void Window::SetShown(bool isShown)
	{
		m_windowInfo.isShown = isShown;
		m_windowInfo.isShown ? glfwShowWindow(m_pWindow) : glfwHideWindow(m_pWindow);
	}
	
	bool Window::IsShown() const
	{
		return m_windowInfo.isShown;
	}
	
	void Window::SetFocused(bool isFocused)
	{
		m_windowInfo.isFocused = isFocused;
		if (m_windowInfo.isFocused)
		{
			glfwFocusWindow(m_pWindow);
		}
	}
	
	bool Window::IsFocused() const
	{
		return m_windowInfo.isFocused;
	}
	
	Vec2ui Window::GetFramebufferSize() const
	{
		int32_t windowSizeX{ 0 };
		int32_t windowSizeY{ 0 };
		glfwGetFramebufferSize(m_pWindow, &windowSizeX, &windowSizeY);
		return { static_cast<uint32_t>(windowSizeX), static_cast<uint32_t>(windowSizeY) };
	}
	
	float Window::GetTime() const
	{
		return m_pWindow != nullptr ? static_cast<float>(glfwGetTime()) : 0.0f;
	}
}