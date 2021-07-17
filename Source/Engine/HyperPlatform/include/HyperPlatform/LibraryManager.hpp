/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Bits.hpp>
#include <HyperCore/Prerequisites.hpp>
#include <cstdint>
#include <string>

namespace HyperPlatform
{
	class CLibraryHandle
	{
	public:
		static constexpr const uint32_t s_index_mask{ HyperCore::Mask<uint32_t>(16) };
		static constexpr const uint32_t s_version_mask{ ~HyperCore::Mask<uint32_t>(16) };

	public:
		explicit CLibraryHandle(uint32_t handle = 0);

		uint32_t handle() const noexcept;

		bool valid() const noexcept;

		uint16_t index() const noexcept;
		uint16_t version() const noexcept;

		bool operator==(const CLibraryHandle& library_handle) const noexcept;
		bool operator!=(const CLibraryHandle& library_handle) const noexcept;

	private:
		uint32_t m_handle{ 0 };
	};

	class CLibraryManager
	{
	public:
		HYPERENGINE_NON_COPYABLE(CLibraryManager);
		HYPERENGINE_NON_MOVEABLE(CLibraryManager);

	public:
		~CLibraryManager();

		static CLibraryHandle load(const std::string& path);
		static void unload(CLibraryHandle library_handle);

		static void* get_function_address(CLibraryHandle library_handle, const std::string& function);

	private:
		static uint32_t s_version;
	};
} // namespace HyperPlatform
