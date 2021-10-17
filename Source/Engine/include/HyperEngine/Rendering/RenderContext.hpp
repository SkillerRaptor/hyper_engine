/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/RenderingApi.hpp"

struct GLFWwindow;

namespace HyperEngine
{
	class IContext;
	class IRenderer;

	class CRenderContext
	{
	public:
		struct SDescription
		{
			GLFWwindow* window{ nullptr };

			ERenderingApi rendering_api{ ERenderingApi::None };
			bool debug_mode{ false };
		};

	public:
		~CRenderContext();

		auto create(const SDescription& description) -> bool;

		auto begin_frame() -> bool;
		auto end_frame() -> bool;
		
	private:
		ERenderingApi m_rendering_api{ ERenderingApi::None };

		IContext* m_native_context{ nullptr };
		IRenderer* m_native_renderer{ nullptr };
	};
} // namespace HyperEngine
