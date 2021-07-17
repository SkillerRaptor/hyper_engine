/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Events/EventManager.hpp>
#include <HyperPlatform/Window.hpp>

namespace HyperRendering
{
	class IContext;
}

namespace HyperEngine
{
	class IApplication;

	class CEngineLoop
	{
	private:
		using CreateContextFunction = HyperRendering::IContext* (*) ();

	public:
		void initialize(IApplication* application);
		void shutdown();

		void run();

	private:
		bool m_running{ false };

		IApplication* m_application{ nullptr };

		HyperCore::CEventManager m_event_manager{};
		HyperPlatform::CWindow m_window{};
	};
} // namespace HyperEngine
