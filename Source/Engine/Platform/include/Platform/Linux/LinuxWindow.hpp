#pragma once

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_LINUX
	#include <Platform/Window.hpp>
	
	struct GLFWwindow;
	
	namespace Platform
	{
		class LinuxWindow : public Window
		{
		public:
			explicit LinuxWindow(const WindowCreateInfo& createInfo);
		
		private:
			GLFWwindow* m_nativeWindow;
		};
	}
#endif
