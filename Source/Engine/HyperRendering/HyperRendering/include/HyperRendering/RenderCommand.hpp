/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperMath/Vector4.hpp>

#include <cstdint>

namespace HyperRendering
{
	struct ClearCommand
	{
		HyperMath::Vec4f color;
	};

	class RenderCommand
	{
	public:
		enum class Type : uint8_t
		{
			Undefined = 0,
			Clear
		};

	public:
		explicit RenderCommand(ClearCommand clear_command);
		
	public:
		Type type{ Type::Undefined };

		union
		{
			ClearCommand as_clear_command;
		} value{};
	};
} // namespace HyperRendering
