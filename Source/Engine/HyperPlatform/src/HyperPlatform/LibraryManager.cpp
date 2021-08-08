/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/LibraryManager.hpp>

namespace HyperPlatform
{
	CLibraryHandle::CLibraryHandle(uint32_t handle)
		: m_handle(handle)
	{
	}

	uint32_t CLibraryHandle::handle() const
	{
		return m_handle;
	}

	bool CLibraryHandle::valid() const
	{
		return m_handle != 0;
	}

	uint16_t CLibraryHandle::index() const
	{
		return (m_handle & s_index_mask) >> 0;
	}

	uint16_t CLibraryHandle::version() const
	{
		return (m_handle & s_version_mask) >> 16;
	}

	bool CLibraryHandle::operator==(const CLibraryHandle& library_handle) const
	{
		return m_handle == library_handle.m_handle;
	}

	bool CLibraryHandle::operator!=(const CLibraryHandle& library_handle) const
	{
		return m_handle != library_handle.m_handle;
	}
} // namespace HyperPlatform
