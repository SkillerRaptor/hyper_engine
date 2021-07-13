/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperPlatform/IWindow.hpp>
#include <HyperRendering/DirectX11/Context.hpp>

namespace HyperRendering
{
	HYPERENGINE_API IContext* create_context()
	{
		return new DirectX11::CContext();
	}

	namespace DirectX11
	{
		bool CContext::initialize(HyperPlatform::IWindow*)
		{
			return false;
		}

		void CContext::shutdown()
		{
		}

		void CContext::update()
		{
		}
		
		IRenderer& CContext::renderer()
		{
			HYPERENGINE_ASSERT_NOT_REACHED();
			throw;
		}
	} // namespace OpenGL33
} // namespace HyperRendering
