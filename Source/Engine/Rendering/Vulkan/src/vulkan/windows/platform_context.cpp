#include <vulkan/windows/platform_context.hpp>

#include <core/Logger.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
namespace rendering::windows
{
	void platform_context::initialize(void* instance, void* native_window)
	{
		m_instance = reinterpret_cast<HINSTANCE>(instance);
		m_window = reinterpret_cast<HWND>(native_window);
	}
	
	void platform_context::shutdown()
	{
	}
	
	const char* platform_context::get_required_extension() const
	{
		return "VK_KHR_win32_surface";
	}
}
#endif
