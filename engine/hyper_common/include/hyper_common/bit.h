/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define hyper_bit$(x) (1ull << (x))
#define hyper_mask$(x) (hyper_bit$((x)) - 1ull)
