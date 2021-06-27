/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperRendering/RenderCommand.hpp>

namespace HyperRendering
{
	ICommand::ICommand(ECommandType type)
		: m_type(type)
	{
	}

	ECommandType ICommand::type() const
	{
		return m_type;
	}

	CClearCommand::CClearCommand(HyperMath::CVec4f clear_color)
		: ICommand(ECommandType::Clear)
		, m_clear_color(clear_color)
	{
	}

	HyperMath::CVec4f CClearCommand::clear_color() const
	{
		return m_clear_color;
	}
} // namespace HyperRendering
