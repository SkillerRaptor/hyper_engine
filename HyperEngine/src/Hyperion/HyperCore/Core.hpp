#pragma once

#include <memory>

namespace Hyperion
{
	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Scope<T> CreateScope(Args&& ... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template<typename T>
	using Ref = std::shared_ptr<T>;

	template<typename T, typename ... Args>
	constexpr Ref<T> CreateRef(Args&& ... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "HyperUtilities/Log.hpp"

#ifdef HP_DEBUG
#define HP_CORE_ASSERT(x, ...) \
	{ \
		if(!(x)) \
		{ \
			HP_CORE_ERROR("Assertion Failed: %", __VA_ARGS__); \
			__debugbreak(); \
		} \
	}

#define HP_CLIENT_ASSERT(x, ...) \
	{ \
		if (!(x)) \
		{ \
			HP_CLIENT_ERROR("Assertion Failed: %", __VA_ARGS__); \
			__debugbreak(); \
		} \
	}
#else
#define HP_CORE_ASSERT
#define HP_CLIENT_ASSERT
#endif
