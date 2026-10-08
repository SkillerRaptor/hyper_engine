/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <fmt/format.h>

#include "hyper_core/types.hpp"

namespace he::core::logger {

enum class Level : u8 {
    Trace = 0,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
};

void initialize(Level level);
void flush();

void log(Level level, std::string_view message);

template <typename... Args>
void info(fmt::format_string<Args...> format, Args &&...args)
{
    log(Level::Info, fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
void warn(fmt::format_string<Args...> format, Args &&...args)
{
    log(Level::Warning, fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
void error(fmt::format_string<Args...> format, Args &&...args)
{
    log(Level::Error, fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
void fatal(fmt::format_string<Args...> format, Args &&...args)
{
    log(Level::Fatal, fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
void debug(fmt::format_string<Args...> format, Args &&...args)
{
    log(Level::Debug, fmt::format(format, std::forward<Args>(args)...));
}

template <typename... Args>
void trace(fmt::format_string<Args...> format, Args &&...args)
{
    log(Level::Trace, fmt::format(format, std::forward<Args>(args)...));
}

} // namespace he::core::logger

#define HE_INFO(...) ::he::core::logger::info(__VA_ARGS__)
#define HE_WARN(...) ::he::core::logger::warn(__VA_ARGS__)
#define HE_ERROR(...) ::he::core::logger::error(__VA_ARGS__)
#define HE_FATAL(...) ::he::core::logger::fatal(__VA_ARGS__)
#define HE_DEBUG(...) ::he::core::logger::debug(__VA_ARGS__)
#define HE_TRACE(...) ::he::core::logger::trace(__VA_ARGS__)
