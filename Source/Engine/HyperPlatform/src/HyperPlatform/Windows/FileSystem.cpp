/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/FileSystem.hpp>
#include <HyperPlatform/PlatformDetection.hpp>
#include <Windows.h>

#ifndef HYPERENGINE_PLATFORM_WINDOWS
#	error This file should only be compiled on linux platforms
#endif

namespace HyperPlatform
{
	std::string FileSystem::s_executable_path;

	std::string FileSystem::executable_path()
	{
		if (s_executable_path.empty())
		{
			char buffer[MAX_PATH + 1] = { 0 };
			GetModuleFileName(nullptr, buffer, MAX_PATH);

			s_executable_path = std::string(buffer);
		}

		return s_executable_path;
	}
} // namespace HyperPlatform
