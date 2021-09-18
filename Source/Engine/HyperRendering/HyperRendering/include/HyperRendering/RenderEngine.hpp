/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/RendererType.hpp>
#include <HyperRenderingBase/IGraphicsContext.hpp>

#include <memory>

namespace HyperRendering
{
	class RenderEngine
	{
	public:
		RenderEngine(HyperGame::EventManager& t_event_manager, HyperPlatform::Window& t_window);

		auto initialize() -> bool;

		auto update() -> void;

	private:
		HyperGame::EventManager& m_event_manager;
		HyperPlatform::Window& m_window;
		std::unique_ptr<IGraphicsContext> m_graphics_context{};
	};
} // namespace HyperRendering
