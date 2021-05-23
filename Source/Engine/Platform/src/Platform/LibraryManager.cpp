#include <Platform/LibraryManager.hpp>

#include <Platform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
	#include <Platform/Windows/WindowsLibraryManager.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
	#include <Platform/Linux/LinuxLibraryManager.hpp>
#endif

namespace Platform
{
	bool LibraryHandle::IsHandleValid() const
	{
		return handle != 0;
	}
	
	uint16_t LibraryHandle::GetIndex() const
	{
		static constexpr const uint32_t s_indexMask{ (1u << 16) - 1 };
		return (handle & s_indexMask) >> 0;
	}
	
	uint16_t LibraryHandle::GetVersion() const
	{
		static constexpr const uint32_t s_versionMask{ ~((1u << 16) - 1) };
		return (handle & s_versionMask) >> 16;
	}
	
	bool LibraryHandle::operator==(const LibraryHandle& libraryHandle) const
	{
		return handle == libraryHandle.handle;
	}
	
	LibraryManager* LibraryManager::Construct()
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new WindowsLibraryManager{};
#elif HYPERENGINE_PLATFORM_LINUX
		return new LinuxLibraryManager{};
#endif
		return nullptr;
	}
}
