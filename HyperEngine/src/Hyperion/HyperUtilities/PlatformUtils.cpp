#include "PlatformUtils.hpp"

#if defined(HP_PLATFORM_WINDOWS)
#include "Platform/OS/Windows/WindowsPlatformUtils.hpp"
#endif

namespace Hyperion
{
	Ref<PlatformUtils> PlatformUtils::m_Instance;

	Ref<PlatformUtils> PlatformUtils::Get()
	{
		if (m_Instance == nullptr)
		{
		#if defined(HP_PLATFORM_WINDOWS)
			m_Instance = CreateRef<WindowsPlatformUtils>();
		#else
			HP_CORE_ASSERT(false, "Unknown Platform!");
		#endif
		}
		return m_Instance;
	}
}
