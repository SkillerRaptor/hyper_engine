/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "core/logger.hpp"

#include <fmt/color.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/basic_file_sink.h>

#include <spdlog/sinks/ansicolor_sink-inl.h>

namespace he::logger
{
    static RefPtr<spdlog::logger> s_logger = nullptr;

    namespace detail
    {
        RefPtr<spdlog::logger> spdlog_logger() { return s_logger; }
    } // namespace detail

    void initialize(const spdlog::level::level_enum level)
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
        s_logger->set_level(level);
        s_logger->flush_on(level);
    }
} // namespace he::logger
