/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperRendering/IContext.hpp"

#include <HyperCore/Assertion.hpp>

namespace HyperRendering
{
	IContext::IContext(HyperPlatform::Window& window)
		: m_window(window)
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
			const auto& render_command = m_render_commands.front();

			std::visit([](auto&& converted_event)
			   {
				   using T = std::decay_t<decltype(converted_event)>;
				   if constexpr (std::is_same_v<T, ClearCommand>)
				   {
					   // TODO(SkillerRaptor): Implementing clear command
				   }
				   else
				   {
					   HYPERENGINE_ASSERT_NOT_REACHED();
				   }
			   }, render_command);

			m_render_commands.pop();
		}
	}
	
	auto IContext::window() const -> HyperPlatform::Window&
	{
		return m_window;
	}
} // namespace HyperRendering
