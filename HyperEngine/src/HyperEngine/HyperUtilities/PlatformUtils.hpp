#pragma once

#include <optional>

#include "HyperCore/Core.hpp"

namespace HyperUtilities
{
	class PlatformUtils
	{
	private:
		static HyperCore::Ref<PlatformUtils> m_Instance;

	public:
		PlatformUtils() = default;
		virtual ~PlatformUtils() = default;

		virtual std::optional<std::string> SaveFile(const char* filter) = 0;
		virtual std::optional<std::string> OpenFile(const char* filter) = 0;

		static HyperCore::Ref<PlatformUtils> Get();
	};
}
