/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperPlatform
{
	class IWindow;
}

namespace HyperRendering::OpenGL46
{
	class IPlatformContext
	{
	public:
		virtual ~IPlatformContext() = default;
		
		virtual bool initialize(HyperPlatform::IWindow* window) = 0;
		virtual void shutdown() = 0;

		virtual void swap_buffers() const = 0;

		static IPlatformContext* construct();
	};
} // namespace HyperRendering::OpenGL33
