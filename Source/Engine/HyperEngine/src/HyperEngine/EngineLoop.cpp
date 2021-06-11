/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperEngine/EngineLoop.hpp>
#include <HyperPlatform/IWindow.hpp>

namespace HyperEngine
{
	void CEngineLoop::initialize(IApplication* application)
	{
		m_application = application;
		
		HyperPlatform::SWindowCreateInfo create_info{};
		create_info.title = "HyperEngine";
		create_info.width = 1280;
		create_info.height = 720;
		
		m_window = HyperPlatform::IWindow::construct();
		m_window->initialize(create_info);
		
		m_running = true;
	}
	
	void CEngineLoop::shutdown()
	{
		m_window->shutdown();
		delete m_window;
	}
	
	void CEngineLoop::run()
	{
		while (m_running)
		{
			m_window->poll_events();
		}
	}
}
