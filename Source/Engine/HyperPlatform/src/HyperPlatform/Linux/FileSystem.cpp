/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/FileSystem.hpp>
#include <HyperPlatform/PlatformDetection.hpp>
#include <cstdlib>

#if !HYPERENGINE_PLATFORM_LINUX
#	error This file should only be compiled on linux platforms
#endif

namespace HyperPlatform
{
	std::string FileSystem::s_executable_path;

	std::string FileSystem::executable_path()
	{
		if (s_executable_path.empty())
		{
			s_executable_path = std::string(realpath("/proc/self/exe", nullptr));
		}

		return s_executable_path;
	}
} // namespace HyperPlatform
