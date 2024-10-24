/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/logger.hpp"

#include <fmt/chrono.h>
#include <fmt/color.h>

#include "hyper_core/assertion.hpp"
#include "hyper_core/string.hpp"

namespace hyper_core
{
    void Logger::internal_log(Logger::Level level, const std::source_location &source_location, std::string_view string)
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

        const std::string_view level_name = [&level]()
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

        const std::string time = fmt::format(fg(fmt::color::dark_gray), "{:%FT%TZ}", local_time);

        const std::string level_string = fmt::format(fg(level_color), "{}", level_name);

        const std::string module = [&source_location]()
        {
            const std::string file_name = source_location.file_name();
            const size_t file_name_length = file_name.size() - file_name.find_last_of("/\\");

            auto extract_path = [&](const std::string_view keyword, const size_t offset) -> std::string
            {
                const size_t position = file_name.rfind(keyword);
                if (position == std::string::npos)
                {
                    return {};
                }

                const size_t start = position + offset;
                if (start + file_name_length < file_name.size())
                {
                    return file_name.substr(start, file_name.size() - start - file_name_length);
                }

                const size_t last_folder_position = file_name.substr(0, position).find_last_of("/\\");
                return file_name.substr(last_folder_position + 1, position - last_folder_position - 1);
            };

            std::string module_path = extract_path("src", 4);
            if (module_path.empty())
            {
                module_path = extract_path("include", 8);
            }

            module_path = string::replace_all(module_path, "/", "::");
            module_path = string::replace_all(module_path, "\\", "::");

            return module_path;
        }();

        const std::string module_string = fmt::format(fg(fmt::color::gray), "{}", module);

        fmt::print(fg(fmt::color::light_gray), "{} {} {} {}\n", time, level_string, module_string, string);
    }
} // namespace hyper_core
