/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IRenderer2D.hpp>

namespace HyperRendering::OpenGL33
{
	class Renderer2D : public IRenderer2D
	{
	public:
		virtual auto clear(const HyperMath::Vec4f& clear_color) -> void override;
	};
} // namespace HyperCore
