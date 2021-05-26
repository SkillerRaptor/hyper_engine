#include <opengl33/windows/platform_context.hpp>

#include <core/logger.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
namespace rendering::windows
{
	void platform_context::initialize(void* instance, void* native_window)
	{
		m_instance = reinterpret_cast<HINSTANCE>(instance);
		m_window = reinterpret_cast<HWND>(native_window);
		
		HDC window_handle{ GetDC(m_window) };
		
		PIXELFORMATDESCRIPTOR pfd =
			{
				sizeof(PIXELFORMATDESCRIPTOR),
				1,
				PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
				PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
				32,                   // Colordepth of the framebuffer.
				0, 0, 0, 0, 0, 0,
				0,
				0,
				0,
				0, 0, 0, 0,
				24,                   // Number of bits for the depthbuffer
				8,                    // Number of bits for the stencilbuffer
				0,                    // Number of Aux buffers in the framebuffer.
				PFD_MAIN_PLANE,
				0,
				0, 0, 0
			};
		
		int letWindowsChooseThisPixelFormat;
		letWindowsChooseThisPixelFormat = ChoosePixelFormat(window_handle, &pfd);
		SetPixelFormat(window_handle, letWindowsChooseThisPixelFormat, &pfd);
		
		m_context = wglCreateContext(window_handle);
		wglMakeCurrent(window_handle, m_context);
		
		MessageBoxA(0, (char*) glGetString(GL_VERSION), "OPENGL VERSION", 0);
	}
	
	void platform_context::shutdown()
	{
		wglDeleteContext(m_context);
	}
	
	void platform_context::swap_buffers()
	{
		HDC window_handle{ GetDC(m_window) };
		wglSwapLayerBuffers(window_handle, WGL_SWAP_MAIN_PLANE);
	}
}
#endif
