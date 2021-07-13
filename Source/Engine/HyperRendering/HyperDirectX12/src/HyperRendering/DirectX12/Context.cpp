/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperPlatform/IWindow.hpp>
#include <HyperRendering/DirectX12/Context.hpp>

namespace HyperRendering
{
	HYPERENGINE_API IContext* create_context()
	{
		return new DirectX12::CContext();
	}

	namespace DirectX12
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
