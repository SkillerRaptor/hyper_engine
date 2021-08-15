/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperMath/Vector4.hpp>

namespace HyperRendering
{
	class IRenderer2D
	{
	public:
		virtual ~IRenderer2D() = default;
		
		virtual auto clear(const HyperMath::Vec4f& clear_color) -> void = 0;
	};
} // namespace HyperCore
