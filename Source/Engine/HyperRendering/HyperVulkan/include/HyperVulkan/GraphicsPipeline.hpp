/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>

#include <volk.h>

namespace HyperRendering::Vulkan
{
	class Context;

	class GraphicsPipeline
	{
	public:
		explicit GraphicsPipeline(Context& context);

		auto initialize() -> HyperCore::Result<void, HyperCore::Errors::ConstructError>;
		auto terminate() -> void;

	private:
		Context& m_context;
	};
} // namespace HyperRendering::Vulkan
