/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperRendering/OpenGL33/Renderer.hpp>
#include <HyperRendering/OpenGL33/PlatformInclude.hpp>

namespace HyperRendering::OpenGL33
{
	void CRenderer::begin_frame(HyperMath::CVec4f clear_color)
	{
		command_clear(clear_color);
	}

	void CRenderer::end_frame()
	{
		while (!m_render_commands.empty())
		{
			const CRenderCommand& render_command = m_render_commands.front();

			switch (render_command.type())
			{
			case CRenderCommand::EType::Clear:
				handle_clear_command(render_command.as_clear_command());
				break;
			default:
				break;
			}

			m_render_commands.pop();
		}
	}

	void CRenderer::command_clear(HyperMath::CVec4f clear_color)
	{
		CRenderCommand::SClearCommand clear_command{};
		clear_command.clear_color = clear_color;
		
		m_render_commands.emplace(clear_command);
	}
	
	void CRenderer::handle_clear_command(const CRenderCommand::SClearCommand& clear_command)
	{
		const HyperMath::CVec4f& clear_color = clear_command.clear_color;
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
} // namespace HyperRendering::OpenGL33
