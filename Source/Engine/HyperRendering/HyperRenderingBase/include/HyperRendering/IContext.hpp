/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperPlatform
{
	class CWindow;
}

namespace HyperRendering
{
	enum class RenderingApi
	{
		DirectX11,
		DirectX12,
		OpenGL33,
		OpenGL46,
		Vulkan
	};
	
	class IContext
	{
	public:
		virtual ~IContext() = default;

		virtual bool initialize(HyperPlatform::CWindow& window) = 0;
		virtual void shutdown() = 0;

		virtual void update() = 0;
	};
} // namespace HyperRendering
