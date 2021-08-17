/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/Context.hpp"

namespace HyperRendering::Vulkan
{
	Context::Context(HyperPlatform::Window& window)
		: IContext(window)
	{
	}

	auto Context::initialize() -> HyperCore::Result<void, HyperCore::ConstructError>
	{
		return {};
	}

	auto Context::update() -> void
	{
	}
} // namespace HyperRendering::Vulkan
