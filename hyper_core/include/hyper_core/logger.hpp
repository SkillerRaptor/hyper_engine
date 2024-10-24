/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <source_location>
#include <string_view>

#include <fmt/base.h>
#include <fmt/format.h>

namespace hyper_core
{
    class Logger
    {
    public:
        enum class Level
        {
            Info,
            Warning,
            Error,
            Fatal,
            Debug,
            Trace,
        };

    public:
        template <typename... Args>
        static void info(const std::source_location &source_location, fmt::format_string<Args...> format, Args &&...args)
        {
            Logger::log(Level::Info, source_location, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void warn(const std::source_location &source_location, fmt::format_string<Args...> format, Args &&...args)
        {
            Logger::log(Level::Warning, source_location, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void error(const std::source_location &source_location, fmt::format_string<Args...> format, Args &&...args)
        {
            Logger::log(Level::Error, source_location, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void fatal(const std::source_location &source_location, fmt::format_string<Args...> format, Args &&...args)
        {
            Logger::log(Level::Fatal, source_location, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void debug(const std::source_location &source_location, fmt::format_string<Args...> format, Args &&...args)
        {
            Logger::log(Level::Debug, source_location, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void trace(const std::source_location &source_location, fmt::format_string<Args...> format, Args &&...args)
        {
            Logger::log(Level::Trace, source_location, format, std::forward<Args>(args)...);
        }

    private:
        template <typename... Args>
        static void log(const Level level, const std::source_location &source_location, fmt::format_string<Args...> format, Args &&...args)
        {
            Logger::internal_log(level, source_location, fmt::format(format, std::forward<Args>(args)...));
        }

        static void internal_log(Level level, const std::source_location &source_location, std::string_view string);
    };
} // namespace hyper_core

#define HE_INFO(format, ...) ::hyper_core::Logger::info(::std::source_location::current(), format, __VA_ARGS__)
#define HE_WARN(format, ...) ::hyper_core::Logger::warn(::std::source_location::current(), format, __VA_ARGS__)
#define HE_ERROR(format, ...) ::hyper_core::Logger::error(::std::source_location::current(), format, __VA_ARGS__)
#define HE_FATAL(format, ...) ::hyper_core::Logger::fatal(::std::source_location::current(), format, __VA_ARGS__)
#define HE_DEBUG(format, ...) ::hyper_core::Logger::debug(::std::source_location::current(), format, __VA_ARGS__)
#define HE_TRACE(format, ...) ::hyper_core::Logger::trace(::std::source_location::current(), format, __VA_ARGS__)
