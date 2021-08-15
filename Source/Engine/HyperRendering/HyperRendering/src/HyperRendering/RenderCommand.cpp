/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperRendering/RenderCommand.hpp"

namespace HyperRendering
{
	RenderCommand::RenderCommand(ClearCommand clear_command)
		: type(Type::Clear)
	{
		value.as_clear_command = clear_command;
	}
}
