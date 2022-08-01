/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_math/trigonometric.h"

float hyper_lerp(float a, float b, float interpolation)
{
	if ((a <= 0 && b >= 0) || (a >= 0 && b <= 0))
	{
		return interpolation * b + (1.0f - interpolation) * a;
	}

	if (interpolation == 1.0f)
	{
		return b;
	}

	const float x = a + interpolation * (b - a);
	if ((interpolation > 1.0f) == (b > a))
	{
		return b < x ? x : b;	
	}

	return b > x ? x : b;
}
