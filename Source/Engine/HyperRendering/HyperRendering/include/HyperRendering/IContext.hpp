/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>
#include <HyperPlatform/Window.hpp>

namespace HyperRendering
{
	class IContext
	{
	public:
		explicit IContext(HyperPlatform::Window& window);
		virtual ~IContext() = default;
		
		virtual auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError> = 0;
		virtual auto update() -> void = 0;

	protected:
		HyperPlatform::Window& m_window;
	};
} // namespace HyperRendering
