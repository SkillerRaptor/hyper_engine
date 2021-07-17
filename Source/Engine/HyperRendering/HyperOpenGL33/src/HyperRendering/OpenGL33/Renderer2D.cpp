/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperRendering/OpenGL33/Renderer2D.hpp>
#include <HyperRendering/OpenGL33/PlatformInclude.hpp>

namespace HyperRendering::OpenGL33
{
	void CRenderer::begin_frame(HyperMath::CVec4f clear_color)
	{
	
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
