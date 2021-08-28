/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/JobSystem.hpp>
#include <HyperCore/Result.hpp>
#include <HyperCore/Events/EventManager.hpp>
#include <HyperPlatform/Window.hpp>

#include <memory>

namespace HyperEngine
{
	class IApplication;

	class EngineLoop
	{
	public:
		explicit EngineLoop(IApplication& application);
		
		auto initialize() -> HyperCore::InitializeResult;
		auto terminate() -> void;
		
		auto run() -> void;

	private:
		IApplication& m_application;
		bool m_running{ false };
		
		HyperCore::EventManager m_event_manager{};
		HyperCore::JobSystem m_job_system{};
		HyperPlatform::Window m_window;
	};
} // namespace HyperEngine
