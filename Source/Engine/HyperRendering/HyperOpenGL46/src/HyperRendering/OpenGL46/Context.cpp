/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperPlatform/IWindow.hpp>
#include <HyperRendering/OpenGL46/Context.hpp>
#include <HyperRendering/OpenGL46/IPlatformContext.hpp>

namespace HyperRendering
{
	HYPERENGINE_API IContext* create_context()
	{
		return new OpenGL46::CContext();
	}

	namespace OpenGL46
	{
		bool CContext::initialize(HyperPlatform::IWindow* window)
		{
			m_platform_context = IPlatformContext::construct();

			if (!m_platform_context->initialize(window))
			{
				return false;
			}

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
		
		IRenderer& CContext::renderer()
		{
			HYPERENGINE_ASSERT_NOT_REACHED();
			return *m_renderer;
		}
	} // namespace OpenGL46
} // namespace HyperRendering
