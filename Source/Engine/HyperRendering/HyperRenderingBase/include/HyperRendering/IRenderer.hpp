/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperMath/Vector4.hpp>

namespace HyperRendering
{
	class IRenderer
	{
	public:
		virtual void begin_frame(HyperMath::CVec4f clear_color) = 0;
		virtual void end_frame() = 0;
	};
}
