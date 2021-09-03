/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/DiscordPresence.hpp"

#include <HyperCore/JobSystem.hpp>
#include <HyperGame/EntitySystem/Registry.hpp>
#include <HyperGame/EventSystem/EventManager.hpp>
#include <HyperPlatform/Window.hpp>
#include <HyperRendering/RenderEngine.hpp>

#include <memory>

namespace HyperEngine
{
	class IApplication;

	class EngineLoop
	{
	public:
		explicit EngineLoop(IApplication& application);
		
		auto initialize() -> bool;
		
		auto run() -> void;

	private:
		auto initialize_event_callbacks() -> void;
		
	private:
		IApplication& m_application;
		bool m_running{ false };

		DiscordPresence m_discord_presence{};
		
		HyperCore::JobSystem m_job_system{};
		HyperGame::EventManager m_event_manager{};
		HyperPlatform::Window m_window;
		
		HyperGame::Registry m_registry{};
		HyperRendering::RenderEngine m_render_engine;
	};
} // namespace HyperEngine
