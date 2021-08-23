/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperVulkan/GraphicsPipeline.hpp"

#include "HyperVulkan/Context.hpp"

namespace HyperRendering::Vulkan
{
	GraphicsPipeline::GraphicsPipeline(Context& context)
		: m_context(context)
	{
		HYPERENGINE_VARIABLE_NOT_USED(m_context);
	}

	auto GraphicsPipeline::initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>
	{
		return {};
	}

	auto GraphicsPipeline::terminate() -> void
	{
	}
} // namespace HyperRendering::Vulkan
