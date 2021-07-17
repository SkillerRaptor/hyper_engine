/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/Window.hpp>
#include <HyperRendering/OpenGL46/Context.hpp>

namespace HyperRendering::OpenGL46
{
	bool CContext::initialize(HyperPlatform::CWindow&)
	{
		return false;
	}

	void CContext::shutdown()
	{
	}

	void CContext::update()
	{
	}
} // namespace HyperRendering::OpenGL46
