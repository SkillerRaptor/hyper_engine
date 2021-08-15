/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperRendering/IContext.hpp"

namespace HyperRendering
{
	IContext::IContext(HyperPlatform::Window& window, IRenderer2D* renderer2d)
		: m_window(window)
		, m_renderer2d(renderer2d)
	{
	}
	
	auto IContext::clear(const HyperMath::Vec4f& clear_color) -> void
	{
		m_renderer2d->clear(clear_color);
	}
} // namespace HyperRendering
