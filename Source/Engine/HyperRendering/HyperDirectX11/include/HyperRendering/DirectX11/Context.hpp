/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperRendering/IContext.hpp>

namespace HyperRendering::DirectX11
{
	class IPlatformContext;

	class CContext final : public IContext
	{
	public:
		virtual bool initialize(HyperPlatform::CWindow& window) override;
		virtual void shutdown() override;

		virtual void update() override;
	};
} // namespace HyperRendering::OpenGL33
