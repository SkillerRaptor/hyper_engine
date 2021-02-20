#include "HyperUtilities/PlatformUtils.hpp"

#if defined(HP_PLATFORM_WINDOWS)
	#include "HyperSystem/HyperWindows/WindowsPlatformUtils.hpp"
#elif defined(HP_PLATFORM_LINUX)
	#include "HyperSystem/HyperLinux/LinuxPlatformUtils.hpp"
#endif

namespace HyperUtilities
{
	HyperCore::Ref<PlatformUtils> PlatformUtils::m_Instance;

	HyperCore::Ref<PlatformUtils> PlatformUtils::Get()
	{
		if (m_Instance == nullptr)
		{
		#if defined(HP_PLATFORM_WINDOWS)
			m_Instance = HyperCore::CreateRef<HyperSystem::WindowsPlatformUtils>();
		#elif defined(HP_PLATFORM_LINUX)
			m_Instance = HyperCore::CreateRef<HyperSystem::LinuxPlatformUtils>();
		#else
			HP_ASSERT(false, "Unknown Platform!");
		#endif
		}
		return m_Instance;
	}
}
