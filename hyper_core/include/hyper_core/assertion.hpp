/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <filesystem>
#include <source_location>
#include <utility>

#include "hyper_core/logger.hpp"
#include "hyper_core/prerequisites.hpp"

#define HE_ASSERT(expression, ...)                                                      \
    do {                                                                                \
        if (!(expression)) [[unlikely]] {                                               \
            const ::std::source_location _location = ::std::source_location::current(); \
            const ::std::filesystem::path _path(_location.file_name());                 \
            HE_FATAL(                                                                   \
                "Assertion failed at {}:{}: {}" __VA_OPT__(": {}"),                     \
                _path.filename().string(),                                              \
                _location.line(),                                                       \
                HE_STRINGIFY(expression) __VA_OPT__(, ::fmt::format(__VA_ARGS__)));     \
            ::he::logger::flush();                                                      \
            ::std::abort();                                                             \
        }                                                                               \
    } while (false)

#ifdef HE_DEBUG_BUILD
#    define HE_DEBUG_ASSERT(expression, ...) HE_ASSERT(expression, __VA_ARGS__)
#else
#    define HE_DEBUG_ASSERT(expression, ...) ((void) 0)
#endif

#define HE_PANIC(...)                                                               \
    do {                                                                            \
        const ::std::source_location _location = ::std::source_location::current(); \
        const ::std::filesystem::path _path(_location.file_name());                 \
        HE_FATAL(                                                                   \
            "Panic at {}:{}" __VA_OPT__(": {}"),                                    \
            _path.filename().string(),                                              \
            _location.line() __VA_OPT__(, ::fmt::format(__VA_ARGS__)));             \
        ::he::logger::flush();                                                      \
        ::std::abort();                                                             \
    } while (false)

#define HE_UNREACHABLE() ::std::unreachable()
