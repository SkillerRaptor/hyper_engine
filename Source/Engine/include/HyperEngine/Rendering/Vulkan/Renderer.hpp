/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Rendering/IRenderer.hpp"

#include <volk.h>

namespace HyperEngine::Vulkan
{
	class CContext;

	class CRenderer : public IRenderer
	{
	public:
		auto create(const SDescription& description) -> bool override;

		auto begin_frame() -> bool override;
		auto end_frame() -> bool override;

	private:
		CContext* m_context{ nullptr };

		uint32_t m_current_swapchain_index{ 0 };
	};
} // namespace HyperEngine::Vulkan
