/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Prerequisites.hpp>
#include <string>

namespace HyperPlatform
{
	class FileSystem
	{
	public:
		HYPERENGINE_NON_COPYABLE(FileSystem);
		HYPERENGINE_NON_MOVEABLE(FileSystem);

	public:
		static std::string executable_path();

	private:
		static std::string s_executable_path;
	};
} // namespace HyperPlatform
