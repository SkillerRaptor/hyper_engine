#include <vulkan/linux/platform_context.hpp>

#if HYPERENGINE_PLATFORM_LINUX
namespace rendering::linux
{
	void platform_context::initialize(void* instance, void* native_window)
	{
		m_display = reinterpret_cast<Display*>(instance);
		m_window = reinterpret_cast<Window>(native_window);
	}
	
	void platform_context::shutdown()
	{
	}
	
	const char* platform_context::get_required_extension() const
	{
		return "VK_KHR_xlib_surface";
	}
}
#endif
