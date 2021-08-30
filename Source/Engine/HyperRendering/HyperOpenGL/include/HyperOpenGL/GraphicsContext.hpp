/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRenderingBase/IGraphicsContext.hpp>

namespace HyperRendering::HyperOpenGL
{
	class GraphicsContext final : public IGraphicsContext
	{
	public:
		GraphicsContext(HyperGame::EventManager& t_event_manager, HyperPlatform::Window& t_window);
		~GraphicsContext() override;
		
		auto initialize() -> HyperCore::InitializeResult override;

		auto update() -> void override;
	};
} // namespace HyperRendering::HyperOpenGL
