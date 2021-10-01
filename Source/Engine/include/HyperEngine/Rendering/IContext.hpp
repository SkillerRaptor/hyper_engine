/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperEngine
{
	class IContext
	{
	public:
		struct SDescription
		{
		};

	public:
		virtual ~IContext() = default;
		
		virtual auto create(const SDescription& description) -> bool = 0;
	};
} // namespace HyperEngine
