/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperRendering/IContext.hpp"

namespace HyperRendering
{
	IContext::IContext(HyperPlatform::Window& window)
		: m_window(window)
	{
	}
} // namespace HyperRendering
