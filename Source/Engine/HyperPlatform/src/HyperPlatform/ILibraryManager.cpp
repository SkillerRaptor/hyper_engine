/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperPlatform/ILibraryManager.hpp>
#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#include <HyperPlatform/Windows/LibraryManager.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#include <HyperPlatform/Linux/LibraryManager.hpp>
#endif

namespace HyperPlatform
{
	bool SLibraryHandle::valid() const noexcept
	{
		return handle != 0;
	}
	
	uint16_t SLibraryHandle::index() const noexcept
	{
		return (handle & s_index_mask) >> 0;
	}
	
	uint16_t SLibraryHandle::version() const noexcept
	{
		return (handle & s_version_mask) >> 16;
	}
	
	bool SLibraryHandle::operator==(const SLibraryHandle& SLibraryHandle) const noexcept
	{
		return handle == SLibraryHandle.handle;
	}
	
	bool SLibraryHandle::operator!=(const SLibraryHandle& SLibraryHandle) const noexcept
	{
		return handle != SLibraryHandle.handle;
	}
	
	ILibraryManager* ILibraryManager::construct()
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new Windows::CLibraryManager();
#elif HYPERENGINE_PLATFORM_LINUX
		return new Linux::CLibraryManager();
#endif
		
		HYPERENGINE_ASSERT_NOT_REACHED();
	}
}
