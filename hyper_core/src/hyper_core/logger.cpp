/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/logger.hpp"

#include <fmt/chrono.h>
#include <fmt/color.h>

namespace hyper_core
{
    void Logger::internal_log(Logger::Level level, std::string_view string)
    {
        const fmt::color level_color = [&level]()
        {
            switch (level)
            {
            case Logger::Level::Info:
                return fmt::color::green;
            case Logger::Level::Warning:
                return fmt::color::gold;
            case Logger::Level::Error:
                return fmt::color::red;
            case Logger::Level::Fatal:
                return fmt::color::crimson;
            case Logger::Level::Debug:
                return fmt::color::blue;
            case Logger::Level::Trace:
                return fmt::color::purple;
            default:
                return fmt::color::white;
            }
        }();

        const char *level_name = [&level]()
        {
            switch (level)
            {
            case Logger::Level::Info:
                return " INFO";
            case Logger::Level::Warning:
                return " WARN";
            case Logger::Level::Error:
                return "ERROR";
            case Logger::Level::Fatal:
                return "FATAL";
            case Logger::Level::Debug:
                return "DEBUG";
            case Logger::Level::Trace:
                return "TRACE";
            default:
                return "Undefined";
            }
        }();

        const std::chrono::time_zone *time_zone = std::chrono::current_zone();
        const std::chrono::system_clock::time_point time_point = std::chrono::system_clock::now();
        const auto local_time = time_zone->to_local(time_point);

        const std::string time = fmt::format(fg(fmt::color::gray), "{:%FT%TZ}", local_time);

        const std::string level_string = fmt::format(fg(level_color), "{}", level_name);

        fmt::print(fg(fmt::color::light_gray), "{} {}: {}\n", time, level_string, string);
    }
} // namespace hyper_core
