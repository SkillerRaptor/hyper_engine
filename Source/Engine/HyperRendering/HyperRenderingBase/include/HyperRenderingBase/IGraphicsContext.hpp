/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperRenderingBase/Resources.hpp"

#include <HyperGame/EventSystem/EventManager.hpp>
#include <HyperPlatform/Window.hpp>

namespace HyperRendering
{
	class IGraphicsContext
	{
	public:
		IGraphicsContext(HyperGame::EventManager& t_event_manager, HyperPlatform::Window& t_window);
		virtual ~IGraphicsContext() = default;
		
		virtual auto initialize() -> bool = 0;
		
		virtual auto update() -> void = 0;

	protected:
		HyperGame::EventManager& m_event_manager;
		HyperPlatform::Window& m_window;
	};
} // namespace HyperRendering
