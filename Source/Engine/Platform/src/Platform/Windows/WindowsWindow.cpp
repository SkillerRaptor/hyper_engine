#include <Platform/Windows/WindowsWindow.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS

#include <Core/Logger.hpp>

namespace Platform
{
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
	{
		switch (message)
		{
		case WM_CLOSE:
		case WM_DESTROY:
			Shutdown();
			break;
		default:
			break;
		}
		
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	WindowsWindow::WindowsWindow(const WindowCreateInfo& createInfo)
	{
		m_title = createInfo.title;
		m_width = createInfo.width;
		m_height = createInfo.height;
		
		m_hInstance = GetModuleHandle(nullptr);
		
		WNDCLASSEX windowClass{};
		windowClass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
		windowClass.cbClsExtra = 0;
		windowClass.cbWndExtra = 0;
		windowClass.hInstance = m_hInstance;
		windowClass.lpfnWndProc = WindowProc;
		windowClass.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
		windowClass.hIconSm = windowClass.hIcon;
		windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
		windowClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
		windowClass.lpszMenuName = nullptr;
		windowClass.lpszClassName = m_title.c_str();
		windowClass.cbSize = sizeof(WNDCLASSEX);
		
		if (!RegisterClassEx(&windowClass))
		{
			Core::Logger::Fatal("Failed to register window class! Error: {}", GetLastError());
			std::exit(1);
		}
		
		RECT windowRect{};
		windowRect.left = 0;
		windowRect.top = 0;
		windowRect.right = static_cast<LONG>(m_width);
		windowRect.bottom = static_cast<LONG>(m_height);
		
		constexpr DWORD style{ WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX };
		AdjustWindowRectEx(&windowRect, style, false, 0);
		
		m_hNativeWindow = CreateWindowEx(
			0, windowClass.lpszClassName, m_title.c_str(), style, 0, 0,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			nullptr, nullptr, m_hInstance, 0);
		if(!m_hNativeWindow)
		{
			Core::Logger::Fatal("Failed to create window! Error: {}", GetLastError());
			std::exit(1);
		}
		
		ShowWindow(m_hNativeWindow, SW_SHOWDEFAULT);
		UpdateWindow(m_hNativeWindow);
		SetForegroundWindow(m_hNativeWindow);
		SetFocus(m_hNativeWindow);
		
		ShowCursor(true);
	}
	
	void WindowsWindow::Shutdown()
	{
		PostQuitMessage(0);
	}
	
	void WindowsWindow::Update()
	{
		MSG message{};
		
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
	}
}
#endif
