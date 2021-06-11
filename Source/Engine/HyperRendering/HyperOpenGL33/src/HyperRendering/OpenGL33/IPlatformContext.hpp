/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperPlatform
{
	class IWindow;
}

namespace HyperRendering::OpenGL33
{
	class IPlatformContext
	{
	public:
		IPlatformContext() = default;
		virtual ~IPlatformContext() = default;
		
		virtual void initialize(HyperPlatform::IWindow* window) = 0;
		virtual void shutdown() = 0;
		
		virtual void swap_buffers() const = 0;
		
		static IPlatformContext* construct();
	};
}
