/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperGame/GameModule.hpp"

#include <HyperCore/Logger.hpp>

namespace HyperEngine
{
	auto GameModule::initialize() -> bool
	{
		Logger::info("Successfully initialized Game Module");

		return true;
	}

	auto GameModule::terminate() -> void
	{
		Logger::info("Successfully terminated Game Module");
	}
	
	auto GameModule::name() const -> const char*
	{
		return "Game";
	}
} // namespace HyperEngine
