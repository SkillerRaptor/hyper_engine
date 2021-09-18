/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/CoreModule.hpp"

#include "HyperCore/JobSystem.hpp"
#include "HyperCore/Logger.hpp"

namespace HyperEngine
{
	auto CoreModule::initialize() -> bool
	{
		JobSystem::initialize();

		Logger::info("Successfully initialized Core Module");

		return true;
	}

	auto CoreModule::terminate() -> void
	{
		JobSystem::terminate();

		Logger::info("Successfully terminated Core Module");
	}
	
	auto CoreModule::name() const -> const char*
	{
		return "Core";
	}
} // namespace HyperEngine
