/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperPlatform/ILibraryManager.hpp>
#include <HyperPlatform/PlatformDetection.hpp>

#if HYPERENGINE_PLATFORM_WINDOWS
#	include <HyperPlatform/Windows/LibraryManager.hpp>
#elif HYPERENGINE_PLATFORM_LINUX
#	include <HyperPlatform/Linux/LibraryManager.hpp>
#endif

namespace HyperPlatform
{
	CLibraryHandle::CLibraryHandle(uint32_t handle)
		: m_handle(handle)
	{
	}

	uint32_t CLibraryHandle::handle() const noexcept
	{
		return m_handle;
	}

	bool CLibraryHandle::valid() const noexcept
	{
		return m_handle != 0;
	}

	uint16_t CLibraryHandle::index() const noexcept
	{
		return (m_handle & s_index_mask) >> 0;
	}

	uint16_t CLibraryHandle::version() const noexcept
	{
		return (m_handle & s_version_mask) >> 16;
	}

	bool CLibraryHandle::operator==(const CLibraryHandle& library_handle) const noexcept
	{
		return m_handle == library_handle.m_handle;
	}

	bool CLibraryHandle::operator!=(const CLibraryHandle& library_handle) const noexcept
	{
		return m_handle != library_handle.m_handle;
	}

	ILibraryManager* ILibraryManager::construct()
	{
#if HYPERENGINE_PLATFORM_WINDOWS
		return new Windows::CLibraryManager();
#elif HYPERENGINE_PLATFORM_LINUX
		return new Linux::CLibraryManager();
#else
		HYPERENGINE_ASSERT_NOT_REACHED();
#endif
	}
} // namespace HyperPlatform
