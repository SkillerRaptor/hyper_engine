/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IContext.hpp>

namespace HyperRendering::OpenGL33
{
	class Context final : public IContext
	{
	public:
		explicit Context(HyperPlatform::Window& window);

		auto initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError> override;
		auto terminate() -> void override;
		auto update() -> void override;
	};
} // namespace HyperRendering::OpenGL33
