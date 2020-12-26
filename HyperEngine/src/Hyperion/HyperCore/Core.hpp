#pragma once

#include <memory>

#if defined(_WIN32)
	#if defined(_WIN64)
		#define HP_PLATFORM_WINDOWS
	#else
		#error "x86 Builds are not supported!"
	#endif
#elif defined(__APPLE__) || defined(__MACH__)
	#include <TargetConditionals.h>

	#if TARGET_IPHONE_SIMULATOR == 1
		#error "IOS simulator is not supported!"
	#elif TARGET_OS_IPHONE == 1
		#define HP_PLATFORM_IOS
		#error "IOS is not supported!"
	#elif TARGET_OS_MAC == 1
		#define HP_PLATFORM_MACOS
		#error "MacOS is not supported!"
	#else
		#error "Unknown Apple platform!"
	#endif
#elif defined(__ANDROID__)
	#define HP_PLATFORM_ANDROID
	#error "Android is not supported!"
#elif defined(__linux__)
	#define HP_PLATFORM_LINUX
#else
	#error "Unknown platform!"
#endif

#ifdef HP_DEBUG
	#if defined(HP_PLATFORM_WINDOWS)
		#define HP_DEBUGBREAK() __debugbreak()
	#elif defined(HP_PLATFORM_LINUX)
		#include <signal.h>
		#define HP_DEBUGBREAK() raise(SIGTRAP)
	#else
		#error "Platform doesn't support debugbreak yet!"
	#endif

	#define HP_ENABLE_ASSERTS
#elif
	#define HP_DEBUGBREAK()
#endif

#ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
	#if defined(__cpp_lib_filesystem)
		#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
	#elif defined(__cpp_lib_experimental_filesystem)
		#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
	#elif !defined(__has_include)
		#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
	#elif __has_include(<filesystem>)
		#ifdef _MSC_VER
			#if __has_include(<yvals_core.h>)
					#include <yvals_core.h>
				#if defined(_HAS_CXX17) && _HAS_CXX17
					#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
				#endif
			#endif

		#ifndef INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
			#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1
		#endif

			#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 0
		#endif

	#elif __has_include(<experimental/filesystem>)
		#define INCLUDE_STD_FILESYSTEM_EXPERIMENTAL 1

	#else
		#error Could not find system header "<filesystem>" or "<experimental/filesystem>"
	#endif

	#if INCLUDE_STD_FILESYSTEM_EXPERIMENTAL
		#include <experimental/filesystem>
		namespace std
		{
			namespace filesystem = experimental::filesystem;
		}
	#else
		#include <filesystem>
	#endif
#endif

#define HP_EXPAND_MACRO(x) x
#define HP_STRINGIFY_MACRO(x) #x

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

#ifdef HP_ENABLE_ASSERTS
	#define HP_INTERNAL_ASSERT_IMPLEMENTATION(check, msg, ...) { if (!(check)) { HP_CORE_FATAL(msg, __VA_ARGS__); HP_DEBUGBREAK(); } }

	#define HP_INTERNAL_ASSERT_WITH_MSG(check, ...) HP_INTERNAL_ASSERT_IMPLEMENTATION(check, "Assertion failed: %", __VA_ARGS__)
	#define HP_INTERNAL_ASSERT_NO_MSG(check) HP_INTERNAL_ASSERT_IMPLEMENTATION(check, "Assertion '%' failed in % at line %", HP_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define HP_INTERNAL_ASSERT_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define HP_INTERNAL_ASSERT_MACRO(...) HP_EXPAND_MACRO( HP_INTERNAL_ASSERT_MACRO_NAME(__VA_ARGS__, HP_INTERNAL_ASSERT_WITH_MSG, HP_INTERNAL_ASSERT_NO_MSG) )

	#define HP_ASSERT(...) HP_EXPAND_MACRO( HP_INTERNAL_ASSERT_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#elif
	#define HP_ASSERT(...)
#endif
