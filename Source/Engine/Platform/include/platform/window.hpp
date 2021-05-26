#pragma once

#include <common/context.hpp>
#include <platform/library_manager.hpp>

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
	protected:
		typedef rendering::context* (*create_context_function)();
		
	public:
		virtual ~window() = default;
		
		virtual bool initialize(library_manager* library_manager) = 0;
		virtual void shutdown() = 0;
		
		virtual void update() = 0;
		
		static window* construct(const window_create_info& createInfo);
	
	protected:
		window() = default;
	
	protected:
		std::string m_title{ "" };
		size_t m_width{ 0 };
		size_t m_height{ 0 };
		graphics_api m_api;
		
		library_manager* m_library_manager;
		library_handle m_graphics_handle;
	
		rendering::context* m_context;
	};
}
