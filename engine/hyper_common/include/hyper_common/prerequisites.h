/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define hyper_array_size$(x) (sizeof(x) / sizeof(*x))

#define hyper_bit$(x) (1ull << x)
#define hyper_mask$(x) (hyper_bit$(x) - 1ull)

// TODO: Moving this into math library
#undef min
#undef max
#define hyper_clamp$(value, min, max) \
	((value < min ? min : value) > max ? max : (value < min ? min : value))
