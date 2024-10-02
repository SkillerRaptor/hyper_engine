/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define HE_STRINGIFY_HELPER(x) #x
#define HE_STRINGIFY(x) HE_STRINGIFY_HELPER(x)
#define HE_EXPAND_MACRO(x) x

#ifndef NDEBUG
#    define HE_DEBUG_BUILD 1
#else
#    define HE_RELEASE_BUILD 1
#endif
