/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperRendering/RenderCommands.hpp"

namespace HyperRendering
{
	ClearCommand::ClearCommand(const HyperMath::Vec4f& t_color)
	: color(t_color)
	{
	}
} // namespace HyperRendering
