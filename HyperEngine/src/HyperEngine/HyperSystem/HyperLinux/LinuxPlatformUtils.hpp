#pragma once

#include "HyperCore/Core.hpp"

#ifdef HP_PLATFORM_LINUX

#include "HyperUtilities/PlatformUtils.hpp"

namespace HyperSystem
{
	class LinuxPlatformUtils : public HyperUtilities::PlatformUtils
	{
	public:
		LinuxPlatformUtils() = default;
		~LinuxPlatformUtils() = default;

		virtual std::optional<std::string> SaveFile(const char* filter) override;
		virtual std::optional<std::string> OpenFile(const char* filter) override;
	};
}

#endif
