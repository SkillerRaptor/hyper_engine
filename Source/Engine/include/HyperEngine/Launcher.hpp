/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Application.hpp"
#include "HyperEngine/EngineLoop.hpp"
#include "HyperEngine/Logger.hpp"
#include "HyperEngine/Support/Concepts.hpp"

#include <type_traits>

namespace HyperEngine
{
	class Launcher
	{
	public:
		template <Derived<Application> T, typename... Args>
		static int launch(Args &&...args)
		{
			T application(std::forward<Args>(args)...);

			auto engine_loop = EngineLoop::create(application);
			if (engine_loop.is_error())
			{
				Logger::error("{}\n", engine_loop.error().error());
				return EXIT_FAILURE;
			}

			engine_loop->run();

			return EXIT_SUCCESS;
		}
	};
} // namespace HyperEngine
