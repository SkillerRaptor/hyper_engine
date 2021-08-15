/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperOpenGL33/Renderer2D.hpp"

#include <HyperRendering/IContext.hpp>

namespace HyperRendering::OpenGL33
{
	class Context : public IContext
	{
	public:
		explicit Context(HyperPlatform::Window& window);
		
		virtual auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError> override;
		virtual auto update() -> void override;
		
		virtual auto clear_implementation(const HyperMath::Vec4f& clear_color) -> void override;
	};
} // namespace HyperRendering::OpenGL33
