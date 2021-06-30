/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Assertion.hpp>
#include <HyperRendering/RenderCommand.hpp>

namespace HyperRendering
{
	CRenderCommand::CRenderCommand(CRenderCommand::SClearCommand clear_command)
		: m_type(EType::Clear)
	{
		m_value.as_clear_command = clear_command;
	}
	
	CRenderCommand::EType CRenderCommand::type() const
	{
		return m_type;
	}

	bool CRenderCommand::is_clear_command() const
	{
		return m_type == EType::Clear;
	}
	
	CRenderCommand::SClearCommand CRenderCommand::as_clear_command() const
	{
		HYPERENGINE_ASSERT(is_clear_command());
		return m_value.as_clear_command;
	}
	
	const char* CRenderCommand::to_string() const
	{
		switch(m_type)
		{
		case EType::Clear:
			return "clear";
		default:
			return "undefined";
		}
	}
} // namespace HyperRendering
