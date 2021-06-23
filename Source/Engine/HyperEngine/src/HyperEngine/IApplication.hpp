/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperEngine
{
	class IApplication
	{
	public:
		friend class CLauncher;

	public:
		virtual ~IApplication() = default;

	protected:
		virtual void startup() = 0;
		virtual void shutdown() = 0;
	};
} // namespace HyperEngine
