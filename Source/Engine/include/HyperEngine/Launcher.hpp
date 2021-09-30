/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/IApplication.hpp"

#include <utility>
#include <type_traits>

namespace HyperEngine
{
	class CLauncher
	{
	public:
		template <typename T>
		static auto launch(T& application) -> int
		{
			static_assert(std::is_base_of_v<IApplication, T>, "'T' is not a derived from IApplication!");

			return launch_application(application);
		}

		template <typename T, typename... Args>
		static auto launch(Args&&... args) -> int
		{
			static_assert(std::is_base_of_v<IApplication, T>, "'T' is not a derived from IApplication!");

			auto application = T(std::forward<Args>(args)...);
			return launch_application(application);
		}

	private:
		static auto launch_application(IApplication& application) -> int;
	};
} // namespace HyperEngine
