/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IContext.hpp>

namespace HyperRendering::Vulkan
{
	class Context final : public IContext
	{
	public:
		explicit Context(HyperPlatform::Window& window);

		auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError> override;
		auto update() -> void override;
	};
} // namespace HyperRendering::Vulkan
