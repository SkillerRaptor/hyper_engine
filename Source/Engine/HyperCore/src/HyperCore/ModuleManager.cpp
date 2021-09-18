/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/ModuleManager.hpp"

#include "HyperCore/Logger.hpp"

namespace HyperEngine
{
	std::vector<ModuleManager::Module> ModuleManager::s_modules{};

	auto ModuleManager::initialize() -> bool
	{
		Logger::debug("Successfully initialized Module Manager");

		return true;
	}

	auto ModuleManager::terminate() -> void
	{
		for (std::vector<ModuleManager::Module>::reverse_iterator it = s_modules.rbegin(); it != s_modules.rend(); ++it)
		{
			IModule* module = it->module;
			module->terminate();
			
			Logger::debug("Unregistered {} module", module->name());

			delete module;
		}

		Logger::debug("Successfully terminated Module Manager");
	}
} // namespace HyperEngine
