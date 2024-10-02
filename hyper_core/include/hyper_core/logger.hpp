/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string_view>

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
        static void info(std::string_view format, Args &&...args)
        {
            Logger::log(Level::Info, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void warn(std::string_view format, Args &&...args)
        {
            Logger::log(Level::Warning, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void error(std::string_view format, Args &&...args)
        {
            Logger::log(Level::Error, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void fatal(std::string_view format, Args &&...args)
        {
            Logger::log(Level::Fatal, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void debug(std::string_view format, Args &&...args)
        {
            Logger::log(Level::Debug, format, std::forward<Args>(args)...);
        }

        template <typename... Args>
        static void trace(std::string_view format, Args &&...args)
        {
            Logger::log(Level::Trace, format, std::forward<Args>(args)...);
        }

    private:
        template <typename... Args>
        static void log(Level level, std::string_view format, Args &&...args)
        {
            if (format.empty())
            {
                Logger::internal_log(level, format);
                return;
            }

            constexpr size_t args_count = sizeof...(Args);
            if constexpr (args_count == 0)
            {
                Logger::internal_log(level, format);
            }
            else
            {
                Logger::internal_log(level, fmt::format(format, std::forward<Args>(args)...));
            }
        }

        static void internal_log(Level level, std::string_view string);
    };
} // namespace hyper_core

#define HE_INFO(...) ::hyper_core::Logger::info(__VA_ARGS__)
#define HE_WARN(...) ::hyper_core::Logger::warn(__VA_ARGS__)
#define HE_ERROR(...) ::hyper_core::Logger::error(__VA_ARGS__)
#define HE_FATAL(...) ::hyper_core::Logger::fatal(__VA_ARGS__)
#define HE_DEBUG(...) ::hyper_core::Logger::debug(__VA_ARGS__)
#define HE_TRACE(...) ::hyper_core::Logger::trace(__VA_ARGS__)
