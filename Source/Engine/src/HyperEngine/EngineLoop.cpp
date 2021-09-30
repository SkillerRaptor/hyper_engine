/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperEngine/EngineLoop.hpp"

#include "HyperEngine/IApplication.hpp"

namespace HyperEngine
{
	auto CEngineLoop::create(const CEngineLoop::SDescription& description) -> bool
	{
		m_application = description.application;
		
		if (m_application == nullptr)
		{
			CLogger::fatal("CEngineLoop::create(): The description application is null!");
			return false;
		}
		
		m_running = false;

		return true;
	}

	auto CEngineLoop::run() -> void
	{
		while (m_running)
		{
			m_application->update();
		}
	}
} // namespace HyperEngine
