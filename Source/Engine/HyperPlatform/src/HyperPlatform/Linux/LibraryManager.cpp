/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Memory/SparsePool.hpp>
#include <HyperPlatform/LibraryManager.hpp>
#include <HyperPlatform/PlatformDetection.hpp>
#include <dlfcn.h>

#if !HYPERENGINE_PLATFORM_LINUX
#	error This file should only be compiled on linux platforms
#endif

namespace HyperPlatform
{
	struct SLibraryData
	{
		uint32_t magic_number{ 0 };

		std::string path;
		void* library{ nullptr };
	};

	static HyperCore::CSparsePool<SLibraryData> s_storage{ 256 };
	uint32_t CLibraryManager::s_version{ 0 };

	CLibraryManager::~CLibraryManager()
	{
		for (size_t i = 0; i < s_storage.size(); ++i)
		{
			dlclose(s_storage[i].library);
		}
		s_storage.clear();
	}

	CLibraryHandle CLibraryManager::load(const std::string& path)
	{
		SLibraryData data{};
		data.magic_number = s_version++;
		data.path = path;
		data.library = dlopen(path.c_str(), RTLD_LAZY);
		if (data.library == nullptr)
		{
			HyperCore::CLogger::error("Failed to load dynamic library: {}!", dlerror());
			return CLibraryHandle(-1);
		}

		uint64_t index = s_storage.push(data);
		return CLibraryHandle((data.magic_number << 16) | static_cast<uint32_t>(index));
	}

	void CLibraryManager::unload(CLibraryHandle handle)
	{
		SLibraryData& data = s_storage[handle.index()];
		if (data.magic_number != handle.version())
		{
			return;
		}

		dlclose(data.library);
		s_storage.remove(handle.index());
	}

	void* CLibraryManager::get_function_address(CLibraryHandle handle, const std::string& function)
	{
		SLibraryData& data = s_storage[handle.index()];
		if (data.magic_number != handle.version())
		{
			return nullptr;
		}

		return dlsym(data.library, function.c_str());
	}
} // namespace HyperPlatform
