/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Events/EventManager.hpp>

namespace HyperEngine
{
	class Application;

	class EngineLoop
	{
	public:
		EngineLoop(Application& application);

		void run();

	private:
		Application& m_application;
		bool m_running{ false };
		
		HyperCore::EventManager m_event_manager{};
	};
} // namespace HyperEngine
