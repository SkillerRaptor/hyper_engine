/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperMath/Vector4.hpp>

namespace HyperRendering
{
	enum class ECommandType
	{
		Clear
	};

	class ICommand
	{
	public:
		explicit ICommand(ECommandType type);
		virtual ~ICommand() = default;

		ECommandType type() const;

	private:
		ECommandType m_type;
	};

	class CClearCommand : public ICommand
	{
	public:
		CClearCommand(HyperMath::CVec4f clear_color);
		
		HyperMath::CVec4f clear_color() const;

	private:
		HyperMath::CVec4f m_clear_color;
	};
} // namespace HyperRendering
