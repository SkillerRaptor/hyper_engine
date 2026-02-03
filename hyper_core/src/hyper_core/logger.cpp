/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/logger.hpp"

#include <fmt/color.h>
#include <spdlog/sinks/ansicolor_sink-inl.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/spdlog.h>

#include "hyper_core/assertion.hpp"
#include "hyper_core/memory.hpp"

namespace he::logger
{
    static RefPtr<spdlog::logger> s_logger = nullptr;

    static spdlog::level::level_enum to_spdlog_level(const Level level)
    {
        switch (level)
        {
        case Level::Info:
            return spdlog::level::info;
        case Level::Warning:
            return spdlog::level::warn;
        case Level::Error:
            return spdlog::level::err;
        case Level::Fatal:
            return spdlog::level::critical;
        case Level::Debug:
            return spdlog::level::debug;
        case Level::Trace:
            return spdlog::level::trace;
        default:
            HE_UNREACHABLE();
        }
    }

    void initialize(const Level level)
    {
        const RefPtr<spdlog::sinks::ansicolor_stdout_sink_mt> stdout_sink
            = make_ref<spdlog::sinks::ansicolor_stdout_sink_mt>();
        stdout_sink->set_color(spdlog::level::info, "\033[38;2;0;128;0m");
        stdout_sink->set_color(spdlog::level::warn, "\033[38;2;255;215;0m");
        stdout_sink->set_color(spdlog::level::err, "\033[38;2;255;0;0m");
        stdout_sink->set_color(spdlog::level::critical, "\033[38;2;220;20;60m");
        stdout_sink->set_color(spdlog::level::debug, "\033[38;2;0;0;255m");
        stdout_sink->set_color(spdlog::level::trace, "\033[38;2;128;0;128m");
        stdout_sink->set_pattern(
            "\033[38;2;69;69;69m%Y-%m-%dT%H:%M:%S.%f %^%l%$\033[38;2;120;120;120m: \033[38;2;211;211;211m%v");

        const RefPtr<spdlog::sinks::basic_file_sink_mt> file_sink
            = make_ref<spdlog::sinks::basic_file_sink_mt>("latest.log", true);
        file_sink->set_pattern("%Y-%m-%d%H:%M:%S.%f %l: %v");

        s_logger = make_ref<spdlog::logger>(
            "HyperEngine",
            spdlog::sinks_init_list {
                stdout_sink,
                file_sink,
            });

        const spdlog::level::level_enum level_value = to_spdlog_level(level);

        s_logger->set_level(level_value);
        s_logger->flush_on(level_value);

        HE_INFO("Initialized logger");
    }

    void log(const Level level, const std::string_view format)
    {
        const spdlog::level::level_enum level_value = to_spdlog_level(level);
        SPDLOG_LOGGER_CALL(s_logger, level_value, format);
    }
} // namespace he::logger
