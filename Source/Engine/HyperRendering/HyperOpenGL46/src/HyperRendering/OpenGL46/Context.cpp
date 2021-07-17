/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperPlatform/IWindow.hpp>
#include <HyperRendering/OpenGL46/Context.hpp>

namespace HyperRendering::OpenGL46
{
	bool CContext::initialize(HyperPlatform::IWindow&)
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
