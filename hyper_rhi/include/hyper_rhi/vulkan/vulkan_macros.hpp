/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <hyper_core/assertion.hpp>

#define HE_VK_CHECK(expression)              \
    do {                                     \
        HE_ASSERT(expression == VK_SUCCESS); \
    } while (false)
