/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperEngine
{
	class IContext;

	class IRenderer
	{
	public:
		struct SDescription
		{
			IContext* context = { nullptr };
		};

	public:
		virtual ~IRenderer() = default;

		virtual auto create(const SDescription& description) -> bool = 0;

		virtual auto begin_frame() -> bool = 0;
		virtual auto end_frame() -> bool = 0;
	};
} // namespace HyperEngine
