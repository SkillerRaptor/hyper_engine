#include <Platform/Linux/LinuxWindow.hpp>

#if HYPERENGINE_PLATFORM_LINUX
	#include <Core/Logger.hpp>
	
	#include <GLFW/glfw3.h>
	
	namespace Platform
	{
		LinuxWindow::LinuxWindow(const WindowCreateInfo& createInfo)
		{
			glfwInit();
			Window::SetWindowHints(createInfo.graphicsApi);
			
			m_nativeWindow = glfwCreateWindow(
				static_cast<int>(createInfo.width),
				static_cast<int>(createInfo.height),
				createInfo.title.c_str(),
				nullptr,
				nullptr);
			
			if (m_nativeWindow == nullptr)
			{
				Core::Logger::Fatal("Failed to create GLFW window!");
				std::exit(1);
			}
		}
	}
#endif
