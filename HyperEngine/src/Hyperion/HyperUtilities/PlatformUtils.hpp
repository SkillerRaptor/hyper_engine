#pragma once

#include "HyperCore/Core.hpp"

namespace Hyperion
{
	class PlatformUtils
	{
	private:
		static Ref<PlatformUtils> m_Instance;

	public:
		virtual std::string SaveFile(const char* filter) = 0;
		virtual std::string OpenFile(const char* filter) = 0;

		static Ref<PlatformUtils> Get();
	};
}
