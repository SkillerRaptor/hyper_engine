/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IContext.hpp>

namespace HyperRendering::OpenGL46
{
	class IPlatformContext;

	class CContext final : public IContext
	{
	public:
		virtual bool initialize(HyperPlatform::IWindow* window) override;
		virtual void shutdown() override;

		virtual void update() override;
		
		virtual IRenderer& renderer() override;

	private:
		IPlatformContext* m_platform_context{ nullptr };
	};
} // namespace HyperRendering::OpenGL46
