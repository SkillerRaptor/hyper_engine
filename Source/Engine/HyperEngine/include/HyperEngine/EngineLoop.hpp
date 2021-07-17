/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Events/EventManager.hpp>

namespace HyperRendering
{
	class IContext;
}

namespace HyperPlatform
{
	class IWindow;
}

namespace HyperEngine
{
	class IApplication;

	class CEngineLoop
	{
	public:
		void initialize(IApplication* application);
		void shutdown();

		void run();

	private:
		bool m_running{ false };

		IApplication* m_application{ nullptr };

		HyperCore::CEventManager m_event_manager{};
		HyperPlatform::IWindow* m_window{ nullptr };
	};
} // namespace HyperEngine
