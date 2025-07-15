/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

using i8 = int8_t;
static_assert(sizeof(i8) == 1);

using i16 = int16_t;
static_assert(sizeof(i16) == 2);

using i32 = int32_t;
static_assert(sizeof(i32) == 4);

using i64 = int64_t;
static_assert(sizeof(i64) == 8);

using u8 = uint8_t;
static_assert(sizeof(u8) == 1);

using u16 = uint16_t;
static_assert(sizeof(u16) == 2);

using u32 = uint32_t;
static_assert(sizeof(u32) == 4);

using u64 = uint64_t;
static_assert(sizeof(u64) == 8);

using f32 = float;
static_assert(sizeof(f32) == 4);

using f64 = double;
static_assert(sizeof(f64) == 8);

using isize = ptrdiff_t;
static_assert(sizeof(isize) == 8);

using usize = size_t;
static_assert(sizeof(usize) == 8);

static_assert(sizeof(isize) == sizeof(usize));
