/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperPlatform/IWindow.hpp>
#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#	include <HyperPlatform/Windows/Window.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#	include <HyperPlatform/Linux/Window.hpp>
#endif

namespace HyperPlatform
{
	IWindow* IWindow::construct()
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new Windows::CWindow();
#elif HYPERENGINE_PLATFORM_LINUX
		return new Linux::CWindow();
#else
		HYPERENGINE_ASSERT_NOT_REACHED();
#endif
	}
} // namespace HyperPlatform
