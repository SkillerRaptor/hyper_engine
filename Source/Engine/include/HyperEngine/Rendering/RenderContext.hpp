/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Core/RenderingApi.hpp"

namespace HyperEngine
{
	class IContext;

	class CRenderContext
	{
	public:
		struct SDescription
		{
			ERenderingApi rendering_api{ ERenderingApi::None };
			bool debug_mode{ false };
		};

	public:
		~CRenderContext();

		auto create(const SDescription& description) -> bool;

	private:
		ERenderingApi m_rendering_api{ ERenderingApi::None };
		
		IContext* m_native_context{ nullptr };
	};
} // namespace HyperEngine
