/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperRendering/OpenGL33/Renderer2D.hpp>
#include <HyperRendering/OpenGL33/PlatformInclude.hpp>

namespace HyperRendering::OpenGL33
{
	void CRenderer::begin_frame()
	{
	}
	
	void CRenderer::begin_frame(HyperMath::CVec4f clear_color)
	{
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void CRenderer::end_frame()
	{
	
	}
	
	void CRenderer::submit_quad(
		const HyperMath::CVec2f& position,
		const HyperMath::CVec2f& size,
		const HyperMath::CVec4f& color)
	{
	}
	
	void CRenderer::submit_quad(const HyperMath::CMat4x4f& transform)
	{
	}
} // namespace HyperRendering::OpenGL33
