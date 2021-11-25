/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Application.hpp"
#include "HyperEngine/EngineLoop.hpp"

#include <type_traits>

namespace HyperEngine
{
	template <typename T>
	concept IsApplication = std::is_base_of_v<Application, T>;

	class Launcher
	{
	public:
		template <IsApplication T, typename... Args>
		static int launch(Args &&...args)
		{
			T application(std::forward<Args>(args)...);

			Expected<EngineLoop> engine_loop = EngineLoop::create(application);
			if (engine_loop.is_error())
			{
				return EXIT_FAILURE;
			}

			engine_loop->run();

			return EXIT_SUCCESS;
		}
	};
} // namespace HyperEngine
