/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define HYPER_MIN(a, b) (((b) < (a)) ? (b) : (a))
#define HYPER_MAX(a, b) (((a) < (b)) ? (b) : (a))

#define HYPER_CLAMP(value, lower, upper) \
	(HYPER_MAX((lower), HYPER_MIN((value), (upper))))
