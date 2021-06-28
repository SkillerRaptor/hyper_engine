/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperMath/Vector4.hpp>

namespace HyperRendering
{
	class CRenderCommand
	{
	public:
		enum class EType
		{
			Undefined,
			Clear
		};

		struct SClearCommand
		{
			HyperMath::CVec4f clear_color;
		};

	public:
		explicit CRenderCommand(SClearCommand clear_command);

		EType type() const;

		bool is_clear_command() const;

		SClearCommand as_clear_command() const;
		
		const char* to_string() const;

	private:
		EType m_type{ EType::Undefined };

		union
		{
			SClearCommand as_clear_command;
		} m_value{};
	};
} // namespace HyperRendering
