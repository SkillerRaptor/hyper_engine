#include "HyperUtilities/PlatformUtils.hpp"

#if defined(HP_PLATFORM_WINDOWS)
	#include "HyperSystem/HyperWindows/WindowsPlatformUtils.hpp"
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
		#else
			HP_ASSERT(false, "Unknown Platform!");
		#endif
		}
		return m_Instance;
	}
}
