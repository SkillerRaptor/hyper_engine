/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define hyper_min$(a, b) (((b) < (a)) ? (b) : (a))
#define hyper_max$(a, b) (((a) < (b)) ? (b) : (a))

#define hyper_clamp$(value, lower, upper) \
	(hyper_max$((lower), hyper_min$((value), (upper))))
