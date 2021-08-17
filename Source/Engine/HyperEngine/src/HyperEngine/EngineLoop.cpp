/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/IApplication.hpp"

#include <HyperOpenGL33/Context.hpp>
#include <HyperVulkan/Context.hpp>

namespace HyperEngine
{
	EngineLoop::EngineLoop(IApplication& application)
		: m_application(application)
		, m_window(m_application.title(), 1280, 720, m_application.graphics_api(), m_event_manager)
	{
	}

	EngineLoop::~EngineLoop()
	{
		delete m_render_context;
	}

	auto EngineLoop::initialize() -> HyperCore::Result<void, HyperCore::ConstructError>
	{
		m_event_manager.register_listener<HyperCore::WindowCloseEvent>(
			"HyperEngine::EngineLoop::CloseEvent",
			[this](const HyperCore::WindowCloseEvent&)
			{
				m_running = false;
			});

		auto window_result = m_window.initialize();
		if (window_result.is_error())
		{
			return window_result.error();
		}

		m_render_context = [this]() -> HyperRendering::IContext*
		{
			switch (m_application.graphics_api())
			{
			case HyperPlatform::GraphicsApi::OpenGL33:
				return new HyperRendering::OpenGL33::Context(m_window);
			case HyperPlatform::GraphicsApi::Vulkan:
				return new HyperRendering::Vulkan::Context(m_window);
			default:
				return nullptr;
			}
		}();

		if (m_render_context == nullptr)
		{
			return HyperCore::ConstructError::UndefinedBehaviour;
		}

		auto render_context_result = m_render_context->initialize();
		if (render_context_result.is_error())
		{
			return render_context_result.error();
		}

		m_running = true;

		return {};
	}

	auto EngineLoop::run() -> void
	{
		float last_time = 0.0f;
		while (m_running)
		{
			float current_time = m_window.time();
			float delta_time = last_time - current_time;
			last_time = current_time;

			(void) delta_time;

			m_event_manager.process_next_event();

			m_render_context->begin_frame();
			m_render_context->submit_command<HyperRendering::ClearCommand>(HyperMath::Vec4f{ 0.1f, 0.1f, 0.1f, 1.0f });
			m_render_context->end_frame();

			m_render_context->update();
			m_window.poll_events();
		}
	}
} // namespace HyperEngine
