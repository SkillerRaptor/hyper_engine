/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace HyperPlatform
{
	enum class GraphicsApi : uint8_t
	{
		DirectX11,
		DirectX12,
		OpenGL33,
		OpenGL46,
		Vulkan
	};
}
