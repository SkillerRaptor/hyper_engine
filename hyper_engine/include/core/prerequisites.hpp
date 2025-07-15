/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <utility>

#define HE_STRINGIFY_HELPER(x) #x
#define HE_STRINGIFY(x) HE_STRINGIFY_HELPER(x)
#define HE_CONCAT_HELPER(a, b) a##b
#define HE_CONCAT(a, b) HE_CONCAT_HELPER(a, b)
#define HE_EXPAND_MACRO(x) x

#ifndef NDEBUG
#    define HE_DEBUG_BUILD 1
#else
#    define HE_RELEASE_BUILD 1
#endif

#define HE_NON_COPYABLE(x) \
    x(x const &) = delete; \
    x &operator=(x const &) = delete

#define HE_NON_MOVABLE(x) \
    x(x &&) = delete;     \
    x &operator=(x &&) = delete

#define HE_BIND_FUNCTION(function)                                    \
    [this](auto &&...args) -> decltype(auto)                          \
    {                                                                 \
        return this->function(std::forward<decltype(args)>(args)...); \
    }

#define HE_UNREACHABLE() std::unreachable()
