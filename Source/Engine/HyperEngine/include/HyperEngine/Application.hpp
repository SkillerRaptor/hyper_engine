/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperEngine
{
	class Application
	{
	public:
		friend class Launcher;

	public:
		virtual ~Application() = default;

	protected:
		virtual void startup() = 0;
		virtual void shutdown() = 0;
	};
} // namespace HyperEngine
