#include <opengl33/linux/platform_context.hpp>

#if HYPERENGINE_PLATFORM_LINUX
namespace rendering::linux
{
	void platform_context::initialize(void* instance, void* native_window)
	{
		m_display = reinterpret_cast<Display*>(instance);
		m_window = reinterpret_cast<Window>(native_window);
		
		GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
		m_visual_info = glXChooseVisual(m_display, 0, att);
		m_context = glXCreateContext(m_display, m_visual_info, nullptr, GL_TRUE);
		glXMakeCurrent(m_display, m_window, m_context);
	}
	
	void platform_context::shutdown()
	{
		glXDestroyContext(m_display, m_context);
	}
	
	void platform_context::swap_buffers()
	{
		glXSwapBuffers(m_display, m_window);
	}
}
#endif
