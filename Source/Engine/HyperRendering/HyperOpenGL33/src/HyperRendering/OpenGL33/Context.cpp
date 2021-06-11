/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Logger.hpp>
#include <HyperPlatform/IWindow.hpp>
#include <HyperRendering/OpenGL33/Context.hpp>
#include <HyperRendering/OpenGL33/IPlatformContext.hpp>

namespace HyperRendering
{
	HYPERENGINE_API IContext* create_context()
	{
		return new OpenGL33::CContext();
	}
	
	namespace OpenGL33
	{
		bool CContext::initialize(HyperPlatform::IWindow* window)
		{
			m_platform_context = IPlatformContext::construct();
			m_platform_context->initialize(window);
			
			return true;
		}
		
		void CContext::shutdown()
		{
			delete m_platform_context;
		}
		
		void CContext::update()
		{
			m_platform_context->swap_buffers();
		}
	}
}
