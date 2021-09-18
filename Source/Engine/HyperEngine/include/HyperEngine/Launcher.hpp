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
		template <typename T>
		static auto launch(T& application) -> int
		{
			static_assert(std::is_base_of<IApplication, T>::value, "'T' is not a base of IApplication!");

			return launch_application(application);
		}

		template <typename T, typename... Args>
		static auto launch(Args&&... args) -> int
		{
			static_assert(std::is_base_of<IApplication, T>::value, "'T' is not a base of IApplication!");

			auto application = T(std::forward<Args>(args)...);
			return launch_application(application);
		}

	private:
		static auto launch_application(IApplication& application) -> int;
	};
} // namespace HyperEngine
