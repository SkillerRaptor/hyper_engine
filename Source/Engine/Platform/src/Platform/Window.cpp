#include <Platform/Window.hpp>

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
	#include <Platform/Linux/LinuxWindow.hpp>
#elif HYPERENGINE_PLATFORM_WINDOWS
	#include <Platform/Windows/WindowsWindow.hpp>
#endif

#include <GLFW/glfw3.h>

namespace Platform
{
	Window* Window::Construct(const WindowCreateInfo& createInfo)
	{
#if HYPERENGINE_PLATFORM_LINUX
		return new LinuxWindow{ createInfo };
#elif HYPERENGINE_PLATFORM_WINDOWS
		return new WindowsWindow{ createInfo };
#endif
		return nullptr;
	}
	
	void Window::SetWindowHints(GraphicsApi graphicsApi)
	{
		switch (graphicsApi)
		{
		case GraphicsApi::OpenGL33:
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			break;
		case GraphicsApi::OpenGL46:
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			break;
		default:
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			break;
		}
	}
}
