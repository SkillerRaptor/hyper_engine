#pragma once

#include <string>

namespace Platform
{
	enum class GraphicsApi
	{
		DirectX11,
		DirectX12,
		OpenGL33,
		OpenGL46,
		Vulkan
	};
	
	struct WindowCreateInfo
	{
		std::string title;
		size_t width;
		size_t height;
		
		GraphicsApi graphicsApi;
	};
	
	class Window
	{
	public:
		virtual ~Window() = default;
		
		virtual void Shutdown() = 0;
		
		virtual void Update() = 0;
		
		static Window* Construct(const WindowCreateInfo& createInfo);
	
	protected:
		Window() = default;
	
	protected:
		std::string m_title{ "" };
		size_t m_width{ 0 };
		size_t m_height{ 0 };
	};
}
