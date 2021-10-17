/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperEngine
{
	enum class ERenderingApi : uint8_t
	{
		None = 0,
		OpenGL,
		Vulkan,

#if HYPERENGINE_BUILD_VULKAN
		Best = Vulkan
#elif HYPERENGINE_BUILD_OPENGL
		Best = OpenGL
#else
		Best = None
#endif
	};
} // namespace HyperEngine
