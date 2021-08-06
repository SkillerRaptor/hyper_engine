/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Events/EventManager.hpp>
#include <HyperPlatform/Window.hpp>

namespace HyperEngine
{
	class IApplication;

	class CEngineLoop
	{
	public:
		CEngineLoop(IApplication& application);
		~CEngineLoop();

		void run();

	private:
		IApplication& m_application;
		bool m_running{ false };
		
		HyperCore::CEventManager m_event_manager{};
		HyperPlatform::CWindow m_window{};
	};
} // namespace HyperEngine
