/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/RendererType.hpp>

#include <string>

namespace HyperEngine
{
	class IApplication
	{
	public:
		virtual ~IApplication() = default;

		virtual auto startup() -> void;
		virtual auto shutdown() -> void;

		virtual auto update() -> void;
	};
} // namespace HyperEngine
