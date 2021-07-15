/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/Windows/LibraryManager.hpp>

namespace HyperPlatform::Windows
{
	CLibraryManager::~CLibraryManager()
	{
		for (size_t i = 0; i < m_storage.size(); ++i)
		{
			internal_unload(m_storage[i]);
		}
		
		m_storage.clear();
	}

	CLibraryHandle CLibraryManager::load(const std::string& path)
	{
		SLibraryData data{};
		data.magic_number = m_version++;
		data.path = path;
		data.library = LoadLibrary(path.c_str());
		uint64_t index = m_storage.push(data);

		if (data.library == nullptr)
		{
			HyperCore::CLogger::error("Failed to load dynamic library: {}!", GetLastError());
			return CLibraryHandle(-1);
		}

		return CLibraryHandle((data.magic_number << 16) | static_cast<uint32_t>(index));
	}

	void CLibraryManager::unload(CLibraryHandle handle)
	{
		SLibraryData& data = m_storage[handle.index()];
		if (data.magic_number != handle.version())
		{
			return;
		}

		CLibraryManager::internal_unload(data);
		m_storage.remove(handle.index());
	}

	void* CLibraryManager::get_function_address(
		CLibraryHandle handle,
		const std::string& function)
	{
		SLibraryData& data = m_storage[handle.index()];
		if (data.magic_number != handle.version())
		{
			return nullptr;
		}

		return reinterpret_cast<void*>(GetProcAddress(data.library, function.c_str()));
	}

	void CLibraryManager::internal_unload(SLibraryData& data)
	{
		FreeLibrary(data.library);
	}
} // namespace HyperPlatform::Windows
