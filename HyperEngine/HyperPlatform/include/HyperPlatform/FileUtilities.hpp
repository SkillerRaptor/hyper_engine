#pragma once

#include <string>
#include <optional>

namespace HyperPlatform
{
	namespace FileUtilities
	{
		std::optional<std::string> SaveFile(const std::string& filter);
		std::optional<std::string> OpenFile(const std::string& filter);
	}
}