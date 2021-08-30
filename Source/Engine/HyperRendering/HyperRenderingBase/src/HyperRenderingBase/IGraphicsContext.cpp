/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperRenderingBase/IGraphicsContext.hpp"

namespace HyperRendering
{
	IGraphicsContext::IGraphicsContext(HyperGame::EventManager& t_event_manager, HyperPlatform::Window& t_window)
		 : m_event_manager(t_event_manager)
		 , m_window(t_window)
	{
	}
} // namespace HyperRendering
