/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperEngine/EngineLoop.hpp>
#include <HyperPlatform/IWindow.hpp>
#include <HyperRendering/IContext.hpp>
#include <HyperRendering/RenderingAPI.hpp>

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
		
		m_library_manager = HyperPlatform::ILibraryManager::construct();
		
		m_graphics_library = m_library_manager->load(HyperRendering::convert_to_library(HyperRendering::RenderingAPI::OpenGL33));
		
		void* create_context_address = m_library_manager->get_function_address(m_graphics_library, "create_context");
		CreateContextFunction create_context = reinterpret_cast<CreateContextFunction>(create_context_address);
		m_graphics_context = create_context();
		m_graphics_context->initialize(m_window);
		
		m_running = true;
	}
	
	void CEngineLoop::shutdown()
	{
		m_graphics_context->shutdown();
		
		m_library_manager->unload(m_graphics_library);
		delete m_library_manager;
		
		m_window->shutdown();
		delete m_window;
	}
	
	void CEngineLoop::run()
	{
		while (m_running)
		{
			m_window->poll_events();
			
			// TODO: Implement Renderer
			
			m_graphics_context->update();
		}
	}
}
