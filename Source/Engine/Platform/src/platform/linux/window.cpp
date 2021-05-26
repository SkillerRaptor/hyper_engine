#include <platform/linux/window.hpp>

#if HYPERENGINE_PLATFORM_LINUX
#include <core/logger.hpp>

namespace platform::linux
{
	window::window(const window_create_info& create_info)
	{
		m_title = create_info.title;
		m_width = create_info.width;
		m_height = create_info.height;
		m_api = create_info.api;
	}
	
	bool window::initialize(library_manager* library_manager)
	{
		m_library_manager = library_manager;
		
		m_display = XOpenDisplay(nullptr);
		if (m_display == nullptr)
		{
			core::logger::fatal("Failed to open X display!");
			return false;
		}
		
		m_screen = DefaultScreen(m_display);
		m_window = XCreateSimpleWindow(
			m_display, RootWindow(m_display, m_screen),
			0, 0, m_width, m_height,
			0, BlackPixel(m_display, m_screen), BlackPixel(m_display, m_screen));
		
		XSelectInput(m_display, m_window, ExposureMask | KeyPressMask);
		XMapWindow(m_display, m_window);
		
		std::string graphics_library{ "" };
		switch (m_api)
		{
		case graphics_api::directx11:
			graphics_library = "./libDirectX11.so";
			break;
		case graphics_api::directx12:
			graphics_library = "./libDirectX12.so";
			break;
		case graphics_api::opengl33:
			graphics_library = "./libOpenGL33.so";
			break;
		case graphics_api::opengl46:
			graphics_library = "./libOpenGL46.so";
			break;
		case graphics_api::vulkan:
			graphics_library = "./libVulkan.so";
			break;
		}
		
		m_graphics_handle = m_library_manager->load(graphics_library);
		
		void* create_context_address{ m_library_manager->get_function(m_graphics_handle, "create_context") };
		create_context_function create_context{ reinterpret_cast<create_context_function>(create_context_address) };
		m_context = create_context();
		m_context->initialize(m_display, reinterpret_cast<void*>(m_window));
		
		return true;
	}
	
	void window::shutdown()
	{
		m_library_manager->unload(m_graphics_handle);
		
		XCloseDisplay(m_display);
	}
	
	void window::update()
	{
		XEvent event{};
		XNextEvent(m_display, &event);
		
		// TODO: Handle Events
		
		m_context->update();
	}
}
#endif
