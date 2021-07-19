/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IRenderer2D.hpp>

namespace HyperRendering::OpenGL33
{
	class CRenderer final : public IRenderer2D
	{
	public:
		virtual void begin_frame() override;
		virtual void begin_frame(HyperMath::CVec4f clear_color) override;
		virtual void end_frame() override;
		
		virtual void submit_quad(
			const HyperMath::CVec2f& position,
			const HyperMath::CVec2f& size,
			const HyperMath::CVec4f& color) override;
		virtual void submit_quad(const HyperMath::CMat4x4f& transform) override;
	};
} // namespace HyperRendering::OpenGL33
