#pragma once

#include "HyperUtilities/PlatformUtils.hpp"

namespace HyperSystem
{
	class WindowsPlatformUtils : public HyperUtilities::PlatformUtils
	{
	public:
		virtual std::optional<std::string> SaveFile(const char* filter) override;
		virtual std::optional<std::string> OpenFile(const char* filter) override;
	};
}
