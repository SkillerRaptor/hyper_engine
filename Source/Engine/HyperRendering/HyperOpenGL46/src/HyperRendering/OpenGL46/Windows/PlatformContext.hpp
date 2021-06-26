/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperPlatform/Windows/Window.hpp>
#include <HyperRendering/OpenGL46/IPlatformContext.hpp>
#include <GL/gl.h>

namespace HyperRendering::OpenGL46::Windows
{
	class CPlatformContext final : public IPlatformContext
	{
	public:
		virtual bool initialize(HyperPlatform::IWindow* window) override;
		virtual void shutdown() override;

		virtual void swap_buffers() const override;

	private:
		HyperPlatform::Windows::CWindow* m_window{ nullptr };

		HGLRC m_graphics_context{ nullptr };
	};
} // namespace HyperRendering::OpenGL46::Windows
