/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IRenderer.hpp>

namespace HyperRendering::OpenGL33
{
	class CRenderer : public IRenderer
	{
	public:
		virtual void begin_frame(HyperMath::CVec4f clear_color) override;
		virtual void end_frame() override;
		
		virtual void command_clear(HyperMath::CVec4f clear_color) override;
		
	private:
		virtual void handle_clear_command(const CRenderCommand::SClearCommand& clear_command) override;
	};
} // namespace HyperRendering::OpenGL33
