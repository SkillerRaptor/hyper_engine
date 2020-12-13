#pragma once

#include "HyperUtilities/PlatformUtils.hpp"

namespace Hyperion
{
	class WindowsPlatformUtils : public PlatformUtils
	{
	public:
		virtual std::string SaveFile(const char* filter) override;
		virtual std::string OpenFile(const char* filter) override;
	};
}
