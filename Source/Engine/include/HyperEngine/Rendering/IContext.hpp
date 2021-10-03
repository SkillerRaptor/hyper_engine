/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

struct GLFWwindow;

namespace HyperEngine
{
	class IContext
	{
	public:
		struct SDescription
		{
			GLFWwindow* window{ nullptr };

			bool debug_mode{ false };
		};

	public:
		virtual ~IContext() = default;

		virtual auto create(const SDescription& description) -> bool = 0;
	};
} // namespace HyperEngine
