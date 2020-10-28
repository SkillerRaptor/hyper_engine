#pragma once

#include <string>

namespace Hyperion
{
	class WindowsFileDialogs
	{
	public:
		static std::string OpenFile(const char* filter);
		static std::string SaveFile(const char* filter);
	};
}
