/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/RenderingApi.hpp"
#include "HyperEngine/Platform/Window.hpp"
#include "HyperEngine/Rendering/RenderContext.hpp"

namespace HyperEngine
{
	class IApplication;

	class CEngineLoop
	{
	public:
		struct SDescription
		{
			IApplication* application;
		};

	public:
		auto create(const SDescription& description) -> bool;

		auto run() -> void;

	private:
		IApplication* m_application{ nullptr };
		
		ERenderingApi m_rendering_api{ ERenderingApi::None };
		CWindow m_window{};
		CRenderContext m_render_context{};
		
		bool m_running{ false };
	};
} // namespace HyperEngine
