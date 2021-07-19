/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperMath/Matrix4x4.hpp>
#include <HyperMath/Vector2.hpp>
#include <HyperRendering/IRenderer.hpp>

namespace HyperRendering
{
	class IRenderer2D : public IRenderer
	{
	public:
		virtual ~IRenderer2D() = default;
		
		virtual void begin_frame() = 0;
		virtual void begin_frame(HyperMath::CVec4f clear_color) = 0;
		virtual void end_frame() = 0;
		
		virtual void submit_quad(
			const HyperMath::CVec2f& position,
			const HyperMath::CVec2f& size,
			const HyperMath::CVec4f& color) = 0;
		virtual void submit_quad(const HyperMath::CMat4x4f& transform) = 0;
	};
}
