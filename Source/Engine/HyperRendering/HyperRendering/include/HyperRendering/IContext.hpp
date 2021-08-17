/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperRendering/RenderCommands.hpp"

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>
#include <HyperPlatform/Window.hpp>

#include <queue>
#include <variant>

namespace HyperRendering
{
	using RenderCommandVariant = std::variant<ClearCommand>;

	class IContext
	{
	public:
		explicit IContext(HyperPlatform::Window& window);
		virtual ~IContext() = default;

		virtual auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError> = 0;
		virtual auto update() -> void = 0;

		auto begin_frame() -> void;
		auto end_frame() -> void;

		template <typename T, typename... Args, typename = std::enable_if_t<std::is_base_of_v<IRenderCommand, T>>>
		auto submit_command(Args&&... args) -> void
		{
			m_render_commands.emplace(T{ std::forward<Args>(args)... });
		}
		
	protected:
		HyperPlatform::Window& m_window;

		std::queue<RenderCommandVariant> m_render_commands{};
	};
} // namespace HyperRendering
