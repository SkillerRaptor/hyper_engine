#pragma once

#include "HyperCore/Core.hpp"

#ifdef HP_PLATFORM_WINDOWS

#include "HyperUtilities/PlatformUtils.hpp"

namespace HyperSystem
{
	class WindowsPlatformUtils : public HyperUtilities::PlatformUtils
	{
	public:
		WindowsPlatformUtils() = default;
		~WindowsPlatformUtils() = default;

		virtual std::optional<std::string> SaveFile(const char* filter) override;
		virtual std::optional<std::string> OpenFile(const char* filter) override;
	};
}

#endif