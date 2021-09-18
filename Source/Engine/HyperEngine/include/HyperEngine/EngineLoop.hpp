/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

namespace HyperEngine
{
	class IApplication;

	class EngineLoop
	{
	public:
		explicit EngineLoop(IApplication& application);
		~EngineLoop();
		
		auto initialize() -> bool;
		
		auto run() -> void;
		
	private:
		IApplication& m_application;
		bool m_running{ false };
	};
} // namespace HyperEngine
