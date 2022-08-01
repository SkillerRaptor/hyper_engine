/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "hyper_common/library.h"
#include "hyper_math/constants.h"

#define HYPER_RADIANS(degrees) ((degrees) * (HYPER_PI / 180.0f))
#define HYPER_DEGREES(radians) ((radians) * (180.0f / HYPER_PI))

HYPER_API float hyper_lerp(float a, float b, float interpolation);
