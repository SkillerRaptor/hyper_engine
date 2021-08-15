/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */
#include <HyperOpenGL33/Renderer2D.hpp>

#include <glad/glad.h>

namespace HyperRendering::OpenGL33
{
	auto Renderer2D::clear(const HyperMath::Vec4f& clear_color) -> void
	{
		glClearColor(clear_color.r, clear_color.g, clear_color.b, clear_color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
}
