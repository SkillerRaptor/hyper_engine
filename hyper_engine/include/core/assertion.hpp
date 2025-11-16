/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "core/logger.hpp"
#include "core/prerequisites.hpp"

#define HE_ASSERT(expression, ...)                                      \
    do                                                                  \
    {                                                                   \
        [[unlikely]] if (!(expression))                                 \
        {                                                               \
            HE_FATAL("Assertion failed: {}", HE_STRINGIFY(expression)); \
            std::abort();                                               \
        }                                                               \
    } while (false)

#ifdef HE_DEBUG_BUILD
#    define HE_DEBUG_ASSERT(expression, ...) HE_ASSERT(expression)
#else
#    define HE_DEBUG_ASSERT(expression, ...) ((void) 0)
#endif
