/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <libassert/assert.hpp>

#define HE_ASSERT(expression, ...) LIBASSERT_ASSERT(expression, __VA_ARGS__)
#define HE_ASSERT_VALUE(expression, ...) LIBASSERT_ASSERT_VAL(expression, __VA_ARGS__)
#define HE_DEBUG_ASSERT(expression, ...) LIBASSERT_DEBUG_ASSERT(expression, __VA_ARGS__)
#define HE_DEBUG_ASSERT_VALUE(expression, ...) LIBASSERT_DEBUG_ASSERT_VAL(expression, __VA_ARGS__)
#define HE_ASSUME(expression, ...) LIBASSERT_ASSUME(expression, __VA_ARGS__)
#define HE_ASSUME_VALUE(expression, ...) LIBASSERT_ASSUME_VAL(expression, __VA_ARGS__)
#define HE_PANIC(...) LIBASSERT_PANIC(__VA_ARGS__)
#define HE_UNREACHABLE(...) LIBASSERT_UNREACHABLE(__VA_ARGS__)