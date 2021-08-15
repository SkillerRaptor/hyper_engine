/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperRendering/IRenderer2D.hpp"
#include "HyperRendering/RenderCommand.hpp"

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>
#include <HyperPlatform/Window.hpp>

#include <queue>

namespace HyperRendering
{
	class IContext
	{
	public:
		IContext(HyperPlatform::Window& window, IRenderer2D* renderer2d);
		virtual ~IContext() = default;

		auto begin_frame() -> void;
		auto end_frame() -> void;

		auto clear(const HyperMath::Vec4f& clear_color) -> void;

		virtual auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError> = 0;
		virtual auto update() -> void = 0;

		template <typename T, typename... Args>
		auto queue_command(Args&&... args) -> void
		{
			m_render_commands.push(RenderCommand(T{ std::forward<Args>(args)... }));
		}

	protected:
		virtual auto clear_implementation(const HyperMath::Vec4f& clear_color) -> void = 0;

	protected:
		HyperPlatform::Window& m_window;
		IRenderer2D* m_renderer2d{ nullptr };

		std::queue<RenderCommand> m_render_commands{};
	};
} // namespace HyperRendering
