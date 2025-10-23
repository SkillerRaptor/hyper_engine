/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <utility>

#ifndef NDEBUG
#    define HE_DEBUG_BUILD 1
#else
#    define HE_RELEASE_BUILD 1
#endif

#if defined(__clang__)
#    define HE_CLANG_COMPILER 1
#elif defined(__GNUC__)
#    define HE_GCC_COMPILER 1
#elif defined(_MSC_VER)
#    define HE_MSVC_COMPILER 1
#else
#    error Unsupported compiler was used
#endif

#define _HE_STRINGIFY_HELPER(x) #x
#define HE_STRINGIFY(x) _HE_STRINGIFY_HELPER(x)

#define _HE_CONCAT_HELPER(a, b) a##b
#define HE_CONCAT(a, b) _HE_CONCAT_HELPER(a, b)

#define HE_EXPAND_MACRO(x) x

#if HE_CLANG_COMPILER || HE_GCC_COMPILER
#    define HE_ALWAYS_INLINE __attribute__((always_inline)) inline
#elif HE_MSVC_COMPILER
#    define HE_ALWAYS_INLINE __forceinline
#endif

#define HE_BIND_FUNCTION(function) \
    [this](auto &&...args) -> decltype(auto) { return this->function(::std::forward<decltype(args)>(args)...); }

#define HE_UNREACHABLE() ::std::unreachable()
