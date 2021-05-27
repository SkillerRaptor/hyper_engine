#include <platform/library_manager.hpp>

#include <platform/platform_detection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <platform/windows/library_manager.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#include <platform/linux/library_manager.hpp>
#endif

namespace platform
{
	bool library_handle::valid() const
	{
		return handle != 0;
	}
	
	uint16_t library_handle::index() const
	{
		static constexpr const uint32_t s_index_mask{ (1u << 16) - 1 };
		return (handle & s_index_mask) >> 0;
	}
	
	uint16_t library_handle::version() const
	{
		static constexpr const uint32_t s_version_mask{ ~((1u << 16) - 1) };
		return (handle & s_version_mask) >> 16;
	}
	
	bool library_handle::operator==(const library_handle& library_handle) const
	{
		return handle == library_handle.handle;
	}
	
	library_manager* library_manager::construct()
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new windows::library_manager{};
#elif HYPERENGINE_PLATFORM_LINUX
		return new linux::library_manager{};
#else
		return nullptr;
#endif
	}
}
