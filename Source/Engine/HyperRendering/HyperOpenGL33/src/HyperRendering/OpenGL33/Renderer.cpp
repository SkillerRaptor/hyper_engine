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
		while (!m_commands.empty())
		{
			ICommand* command = m_commands.front();

			switch (command->type())
			{
			case ECommandType::Clear:
				handle_clear_command(*static_cast<CClearCommand*>(command));
				break;
			default:
				break;
			}

			delete command;
			m_commands.pop();
		}
	}

	void CRenderer::command_clear(HyperMath::CVec4f clear_color)
	{
		m_commands.push(new CClearCommand(clear_color));
	}
	
	void CRenderer::handle_clear_command(const CClearCommand& clear_command)
	{
		const HyperMath::CVec4f clear_color = clear_command.clear_color();
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
} // namespace HyperRendering::OpenGL33
