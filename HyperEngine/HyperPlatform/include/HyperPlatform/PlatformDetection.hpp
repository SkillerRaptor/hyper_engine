#pragma once

/*
 * CPU detection
 */
#if defined(__x86_64__) || defined(_M_X64)
	#define HYPERENGINE_PLATFORM_X64 1
	#define HYPERENGINE_PLATFORM_SSE2 1
#elif defined(__i386) || defined(_M_IX86) || defined(__arm__)
	#error 32-bit platforms are not supported.
#elif defined(__aarch64__)
	#define HYPERENGINE_PLATFORM_ARM 1
#else
	#define HYPERENGINE_PLATFORM_UNKNOWN_CPU 1
#endif

/*
 * Platform detection
 */
#if defined(__APPLE__) || defined(__MACH__) /* Apple */
	#include <TargetConditionals.h>
	
	#define HYPERENGINE_PLATFORM_POSIX 1
	#define HYPERENGINE_PLATFORM_APPLE 1
	
	#if TARGET_OS_IPHONE
		#define HYPERENGINE_PLATFORM_MOBILE 1
		#define HYPERENGINE_PLATFORM_IOS 1
		#if !HYPERENGINE_PLATFORM_UNKNOWN_CPU
			#error iOS: Unsupported CPU
		#endif
	#elif TARGET_OS_MAC
		#define HYPERENGINE_PLATFORM_DESKTOP 1
		#define HYPERENGINE_PLATFORM_MAC 1
		#if !HYPERENGINE_PLATFORM_X64
			#error Unsupported Mac CPU (the only supported is x86-64).
		#endif
	#else
		#error Unknown Apple platform.
	#endif
	
	#error OS-X is not supported in the moment.
#elif defined(ANDROID) || defined(__ANDROID__) /* Android */
	#define HYPERENGINE_PLATFORM_MOBILE 1
	#define HYPERENGINE_PLATFORM_ANDROID 1
	#define HYPERENGINE_PLATFORM_POSIX 1

	#error Android is not supported in the moment.
#elif defined(_WIN32) /* Windows */
	#define HYPERENGINE_PLATFORM_DESKTOP 1
	#define HYPERENGINE_PLATFORM_WINDOWS 1
	#define HYPERENGINE_PLATFORM_WINAPI 1
	
	#if !defined(_WIN64)
		#error Unsupported Windows 64 CPU (the only supported is x86-64).
	#endif
#elif defined(__linux__) || defined(__linux) /* Linux */
	#define HYPERENGINE_PLATFORM_DESKTOP 1
	#define HYPERENGINE_PLATFORM_LINUX 1
	#define HYPERENGINE_PLATFORM_POSIX 1
	
	#if !HYPERENGINE_PLATFORM_X64
		#error Unsupported Linux CPU (the only supported are x86-64).
	#endif
#else
	#error Unknown target platform.
#endif