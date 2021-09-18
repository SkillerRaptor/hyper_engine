/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperResource/ResourceModule.hpp"

#include "HyperResource/ShaderCompiler.hpp"

#include <HyperCore/Logger.hpp>

namespace HyperEngine
{
	auto ResourceModule::initialize() -> bool
	{
		if (!ShaderCompiler::initialize())
		{
			Logger::fatal("ResourceModule::initialize(): Failed to initialize shader compiler");
			return false;
		}

		Logger::info("Successfully initialized Resource Module");

		return true;
	}

	auto ResourceModule::terminate() -> void
	{
		ShaderCompiler::terminate();

		Logger::info("Successfully terminated Resource Module");
	}
	
	auto ResourceModule::name() const -> const char*
	{
		return "Resource";
	}
} // namespace HyperEngine
