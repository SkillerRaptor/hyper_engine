/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <libassert/assert.hpp>

#define HE_ASSERT(...) LIBASSERT_ASSERT(__VA_ARGS__)
#define HE_DEBUG_ASSERT(...) LIBASSERT_DEBUG_ASSERT(__VA_ARGS__)
#define HE_ASSUME(...) LIBASSERT_ASSUME(__VA_ARGS__)
#define HE_PANIC(...) LIBASSERT_PANIC(__VA_ARGS__)
#define HE_UNREACHABLE(...) LIBASSERT_UNREACHABLE(__VA_ARGS__)
