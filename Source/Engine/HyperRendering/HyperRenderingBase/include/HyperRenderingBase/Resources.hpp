/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperRendering
{
	struct ShaderDescription
	{
	};

	struct Shader
	{
		ShaderDescription description{};
		void* internal_state{ nullptr };
	};

	struct TextureDescription
	{
		unsigned int width;
		unsigned int height;
	};

	struct Texture
	{
		TextureDescription description{};
		void* internal_state{ nullptr };
	};
} // namespace HyperRendering
