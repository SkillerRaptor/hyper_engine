/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperMath/MathModule.hpp"

#include <HyperCore/Logger.hpp>

namespace HyperEngine
{
	auto MathModule::initialize() -> bool
	{
		Logger::info("Successfully initialized Math Module");

		return true;
	}

	auto MathModule::terminate() -> void
	{
		Logger::info("Successfully terminated Math Module");
	}
	
	auto MathModule::name() const -> const char*
	{
		return "Math";
	}
} // namespace HyperEngine
