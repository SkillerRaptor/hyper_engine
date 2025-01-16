/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/logger.hpp"

#include <fmt/color.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/ansicolor_sink.h>
#include <spdlog/sinks/ansicolor_sink-inl.h>

namespace hyper_engine
{
    Logger::Logger()
    {
        const auto stdout_sink = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
        stdout_sink->set_color(spdlog::level::info, "\033[38;2;0;128;0m");
        stdout_sink->set_color(spdlog::level::warn, "\033[38;2;255;215;0m");
        stdout_sink->set_color(spdlog::level::err, "\033[38;2;255;0;0m");
        stdout_sink->set_color(spdlog::level::critical, "\033[38;2;220;20;60m");
        stdout_sink->set_color(spdlog::level::debug, "\033[38;2;0;0;255m");
        stdout_sink->set_color(spdlog::level::trace, "\033[38;2;128;0;128m");

        stdout_sink->set_pattern("\033[38;2;69;69;69m%Y-%m-%dT%H:%M:%S.%f %^%l%$ \033[38;2;120;120;120m%s:%#: \033[38;2;211;211;211m%v");

        const auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>("latest.log", true);
        file_sink->set_pattern("%Y-%m-%d%H:%M:%S.%f %l %s:%#: %v");

        m_internal_logger = std::make_unique<spdlog::logger>(
            "HyperEngine",
            spdlog::sinks_init_list{
                stdout_sink,
                file_sink,
            });
        m_internal_logger->set_level(spdlog::level::info);
        m_internal_logger->flush_on(spdlog::level::info);
    }

    void Logger::set_level(const spdlog::level::level_enum level) const
    {
        m_internal_logger->set_level(level);
    }

    const std::unique_ptr<spdlog::logger> &Logger::internal_logger() const
    {
        return m_internal_logger;
    }
} // namespace hyper_engine
