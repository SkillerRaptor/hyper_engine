/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/IApplication.hpp"

#include <HyperCore/CoreModule.hpp>
#include <HyperCore/ModuleManager.hpp>
#include <HyperMath/MathModule.hpp>
#include <HyperGame/GameModule.hpp>
#include <HyperPlatform/PlatformModule.hpp>
#include <HyperResource/ResourceModule.hpp>

namespace HyperEngine
{
	EngineLoop::EngineLoop(IApplication& application)
		: m_application(application)
	{
	}

	EngineLoop::~EngineLoop()
	{
		ModuleManager::terminate();
	}

	auto EngineLoop::initialize() -> bool
	{
		if (!ModuleManager::initialize())
		{
			Logger::fatal("EngineLoop::initialize(): Failed to initialize Module Manager");
			return false;
		}

		ModuleManager::register_module<CoreModule>();

		ModuleManager::register_module<MathModule>();
		ModuleManager::register_module<PlatformModule>();

		ModuleManager::register_module<GameModule>();
		ModuleManager::register_module<ResourceModule>();

		/*
		GameModule& game_module = ModuleManager::get_module<GameModule>();
		game_module.event_manager().register_listener<HyperGame::WindowCloseEvent>(
			[this](const WindowCloseEvent& window_close_event)
			{
				HYPERENGINE_VARIABLE_NOT_USED(window_close_event);

				m_running = false;
			});
		*/

		m_running = true;

		return true;
	}

	auto EngineLoop::run() -> void
	{
		float last_time = 0.0F;
		//while (m_running)
		{
			float current_time = 0.0F; // TODO(SkillerRaptor): Getting window time
			float delta_time = last_time - current_time;
			last_time = current_time;

			HYPERENGINE_VARIABLE_NOT_USED(delta_time);

			// TODO(SkillerRaptor): Updating modules

			m_application.update();
		}
	}
} // namespace HyperEngine
