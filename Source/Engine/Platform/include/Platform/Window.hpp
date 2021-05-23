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
		
		static Window* Construct(const WindowCreateInfo& createInfo);
	
	protected:
		Window() = default;
		
		void SetWindowHints(GraphicsApi graphicsApi);
	};
}
