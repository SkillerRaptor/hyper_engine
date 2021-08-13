/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Errors.hpp>
#include <HyperCore/Result.hpp>
#include <HyperCore/Events/EventManager.hpp>
#include <HyperPlatform/Window.hpp>

namespace HyperEngine
{
	class Application;

	class EngineLoop
	{
	public:
		explicit EngineLoop(Application& application);
		
		auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError>;
		
		auto run() -> void;

	private:
		Application& m_application;
		bool m_running{ false };
		
		HyperCore::EventManager m_event_manager{};
		HyperPlatform::Window m_window;
	};
} // namespace HyperEngine
