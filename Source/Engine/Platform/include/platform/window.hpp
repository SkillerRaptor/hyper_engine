#pragma once

#include <string>

namespace platform
{
	enum class graphics_api
	{
		directx11,
		directx12,
		opengl33,
		opengl46,
		vulkan
	};
	
	struct window_create_info
	{
		std::string title;
		size_t width;
		size_t height;
		
		graphics_api api;
	};
	
	class window
	{
	public:
		virtual ~window() = default;
		
		virtual bool initialize() = 0;
		virtual void shutdown() = 0;
		
		virtual void update() = 0;
		
		static window* construct(const window_create_info& createInfo);
	
	protected:
		window() = default;
	
	protected:
		std::string m_title{ "" };
		size_t m_width{ 0 };
		size_t m_height{ 0 };
	};
}
