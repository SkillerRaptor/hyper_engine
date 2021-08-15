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
#include <HyperRendering/IContext.hpp>

namespace HyperEngine
{
	class IApplication;

	class EngineLoop
	{
	public:
		explicit EngineLoop(IApplication& application);
		~EngineLoop();
		
		auto initialize() -> HyperCore::Result<void, HyperCore::ConstructError>;
		
		auto run() -> void;

	private:
		IApplication& m_application;
		bool m_running{ false };
		
		HyperCore::EventManager m_event_manager{};
		HyperPlatform::Window m_window;
		HyperRendering::IContext* m_render_context{ nullptr };
	};
} // namespace HyperEngine
