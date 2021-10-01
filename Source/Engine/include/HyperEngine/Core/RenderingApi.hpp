/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperEngine
{
	enum class RenderingApi
	{
		None = 0,
		OpenGL,
		Vulkan,

#if HYPERENGINE_BUILD_VULKAN
		Current = Vulkan
#elif HYPERENGINE_BUILD_OPENGL
		Current = OpenGL
#else
		Current = None
#endif
	};
} // namespace HyperEngine
