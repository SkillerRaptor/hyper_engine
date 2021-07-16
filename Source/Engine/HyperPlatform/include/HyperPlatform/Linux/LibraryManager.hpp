/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Memory/SparsePool.hpp>
#include <HyperPlatform/ILibraryManager.hpp>
#include <HyperPlatform/PlatformDetection.hpp>

#ifndef HYPERENGINE_PLATFORM_LINUX
#	error This file should only be included on linux platforms
#endif

namespace HyperPlatform::Linux
{
	class CLibraryManager final : public ILibraryManager
	{
	private:
		struct SLibraryData
		{
			uint32_t magic_number{ 0 };

			std::string path;
			void* library{ nullptr };
		};

	public:
		CLibraryManager() = default;
		virtual ~CLibraryManager() override;

		virtual CLibraryHandle load(const std::string& path) override;
		virtual void unload(CLibraryHandle library_handle) override;

		virtual void* get_function_address(
			CLibraryHandle library_handle,
			const std::string& function) override;

	private:
		static void internal_unload(SLibraryData& data);

	private:
		HyperCore::CSparsePool<SLibraryData> m_storage{ 256 };
		uint32_t m_version{ 1 };
	};
} // namespace HyperPlatform::Linux
