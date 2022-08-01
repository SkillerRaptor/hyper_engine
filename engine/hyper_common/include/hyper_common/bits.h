/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdint.h>

#define HYPER_BIT(x) (1ull << (x))
#define HYPER_BIT_MASK(x) (HYPER_BIT((x)) - 1ull)

#define HYPER_BITS_32_UPPER(x) ((uint32_t) (((x) >> 16) >> 16))
#define HYPER_BITS_32_LOWER(x) ((uint32_t) ((x) & 0xffffffff))

#define HYPER_BITS_16_UPPER(x) ((uint16_t) ((x) >> 16))
#define HYPER_BITS_16_LOWER(x) ((uint16_t) ((x) & 0xffff))

#define HYPER_BYTE_REPEAT(x) ((~0ull / 0xff) * (x))
