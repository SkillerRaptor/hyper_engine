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

	auto IContext::begin_frame() -> void
	{
		while (!m_render_commands.empty())
		{
			m_render_commands.pop();
		}
	}

	auto IContext::end_frame() -> void
	{
		while (!m_render_commands.empty())
		{
			auto render_command = m_render_commands.front();
			m_render_commands.pop();

			switch (render_command.type)
			{
			case RenderCommand::Type::Clear:
			{
				auto clear_command = render_command.value.as_clear_command;
				clear_implementation(clear_command.color);
				break;
			}
			default:
				break;
			}
		}
	}
	
	auto IContext::clear(const HyperMath::Vec4f& clear_color) -> void
	{
		queue_command<ClearCommand>(clear_color);
	}
} // namespace HyperRendering
