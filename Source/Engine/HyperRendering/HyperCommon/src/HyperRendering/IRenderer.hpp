/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperMath/Vector4.hpp>
#include <HyperRendering/RenderCommand.hpp>
#include <queue>

namespace HyperRendering
{
	class IRenderer
	{
	public:
		virtual ~IRenderer() = default;
		
		virtual void begin_frame(HyperMath::CVec4f clear_color) = 0;
		virtual void end_frame() = 0;
		
		virtual void command_clear(HyperMath::CVec4f clear_color) = 0;

	protected:
		virtual void handle_clear_command(const CRenderCommand::SClearCommand& clear_command) = 0;

	protected:
		std::queue<CRenderCommand> m_render_commands;
	};
} // namespace HyperRendering
