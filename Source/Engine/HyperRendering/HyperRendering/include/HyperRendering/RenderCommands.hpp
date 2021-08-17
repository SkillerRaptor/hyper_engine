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
	struct IRenderCommand
	{
	};

	struct ClearCommand final : public IRenderCommand
	{
		explicit ClearCommand(const HyperMath::Vec4f& t_color);
		
		HyperMath::Vec4f color;
	};
} // namespace HyperRendering
