#include <platform/windows/window.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <core/logger.hpp>

namespace platform::windows
{
	LRESULT CALLBACK win_proc(HWND window, UINT message, WPARAM first_parameter, LPARAM second_parameter)
	{
		// TODO: Handle Events
		
		switch (message)
		{
		case WM_CLOSE:
		case WM_DESTROY:
			break;
		default:
			break;
		}
		
		return DefWindowProc(window, message, first_parameter, second_parameter);
	}
	
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
		
		m_instance = GetModuleHandle(nullptr);
		
		WNDCLASSEX window_class{};
		window_class.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		window_class.cbClsExtra = 0;
		window_class.cbWndExtra = 0;
		window_class.hInstance = m_instance;
		window_class.lpfnWndProc = win_proc;
		window_class.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		window_class.hIconSm = window_class.hIcon;
		window_class.hCursor = LoadCursor(nullptr, IDC_ARROW);
		window_class.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		window_class.lpszMenuName = nullptr;
		window_class.lpszClassName = m_title.c_str();
		window_class.cbSize = sizeof(WNDCLASSEX);
		
		if (!RegisterClassEx(&window_class))
		{
			core::logger::fatal("Failed to register window class! Error: {}", GetLastError());
			return false;
		}
		
		RECT window_rect{};
		window_rect.left = 0;
		window_rect.top = 0;
		window_rect.right = static_cast<LONG>(m_width);
		window_rect.bottom = static_cast<LONG>(m_height);
		
		constexpr DWORD style{ WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };
		AdjustWindowRectEx(&window_rect, style, false, 0);
		
		m_native_window = CreateWindowEx(
			0, window_class.lpszClassName, m_title.c_str(), style, 0, 0,
			window_rect.right - window_rect.left, window_rect.bottom - window_rect.top,
			nullptr, nullptr, m_instance, 0);
		if (!m_native_window)
		{
			core::logger::fatal("Failed to create window! Error: {}", GetLastError());
			return false;
		}
		
		ShowWindow(m_native_window, SW_SHOWDEFAULT);
		UpdateWindow(m_native_window);
		SetForegroundWindow(m_native_window);
		SetFocus(m_native_window);
		
		ShowCursor(true);
		
		std::string graphics_library{ "" };
		switch (m_api)
		{
		case graphics_api::directx11:
			graphics_library = "DirectX11.dll";
			break;
		case graphics_api::directx12:
			graphics_library = "DirectX12.dll";
			break;
		case graphics_api::opengl33:
			graphics_library = "OpenGL33.dll";
			break;
		case graphics_api::opengl46:
			graphics_library = "OpenGL46.dll";
			break;
		case graphics_api::vulkan:
			graphics_library = "Vulkan.dll";
			break;
		}
		
		m_graphics_handle = m_library_manager->load(graphics_library);
		
		void* create_context_address{ m_library_manager->get_function(m_graphics_handle, "create_context") };
		create_context_function create_context{ reinterpret_cast<create_context_function>(create_context_address) };
		m_context = create_context();
		m_context->initialize(m_native_window);
		
		return true;
	}
	
	void window::shutdown()
	{
		m_library_manager->unload(m_graphics_handle);
		
		PostQuitMessage(0);
	}
	
	void window::update()
	{
		MSG message{};
		
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		
		m_context->update();
	}
}
#endif
