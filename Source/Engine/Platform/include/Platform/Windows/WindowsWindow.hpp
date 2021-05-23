#pragma once

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
	#include <Platform/Window.hpp>
	
	struct GLFWwindow;
	
	namespace Platform
	{
		class WindowsWindow : public Window
		{
		public:
			explicit WindowsWindow(const WindowCreateInfo& createInfo);
		
		private:
			GLFWwindow* m_nativeWindow;
		};
	}
#endif
