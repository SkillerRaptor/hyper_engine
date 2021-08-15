/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperRendering/IRenderer2D.hpp"

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>
#include <HyperPlatform/Window.hpp>

namespace HyperRendering
{
	class IContext
	{
	public:
		IContext(HyperPlatform::Window& window, IRenderer2D* renderer2d);
		virtual ~IContext() = default;
		
		virtual auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError> = 0;
		virtual auto update() -> void = 0;

		auto clear(const HyperMath::Vec4f& clear_color) -> void;
		
	protected:
		IRenderer2D* m_renderer2d{ nullptr };
		HyperPlatform::Window& m_window;
	};
} // namespace HyperRendering
