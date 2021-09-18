/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/IModule.hpp"

namespace HyperEngine
{
	auto IModule::initialize() -> bool
	{
		return true;
	}

	auto IModule::terminate() -> void
	{
	}
	
	auto IModule::update() -> void
	{
	}
} // namespace HyperEngine
