/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/IApplication.hpp"

#include <type_traits>

namespace HyperEngine
{
	class Launcher
	{
	public:
		template <typename T, typename = std::enable_if_t<std::is_base_of_v<IApplication, T>>>
		static auto launch(T& application) -> int
		{
			return launch_application(application);
		}
		
		template <typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<IApplication, T>>>
		static auto launch(Args&&... args) -> int
		{
			auto application = T(std::forward<Args>(args)...);
			return launch_application(application);
		}

	private:
		static auto launch_application(IApplication& application) -> int;
	};
} // namespace HyperEngine
