/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <spdlog/spdlog.h>

#include "hyper_core/memory.hpp"

namespace he::logger
{
    namespace detail
    {
        RefPtr<spdlog::logger> spdlog_logger();
    } // namespace detail

    void initialize(spdlog::level::level_enum);
} // namespace he::logger

#define HE_INFO(...) SPDLOG_LOGGER_CALL(::he::logger::detail::spdlog_logger(), spdlog::level::info, __VA_ARGS__)
#define HE_WARN(...) SPDLOG_LOGGER_CALL(::he::logger::detail::spdlog_logger(), spdlog::level::warn, __VA_ARGS__)
#define HE_ERROR(...) SPDLOG_LOGGER_CALL(::he::logger::detail::spdlog_logger(), spdlog::level::err, __VA_ARGS__)
#define HE_FATAL(...) SPDLOG_LOGGER_CALL(::he::logger::detail::spdlog_logger(), spdlog::level::critical, __VA_ARGS__)
#define HE_DEBUG(...) SPDLOG_LOGGER_CALL(::he::logger::detail::spdlog_logger(), spdlog::level::debug, __VA_ARGS__)
#define HE_TRACE(...) SPDLOG_LOGGER_CALL(::he::logger::detail::spdlog_logger(), spdlog::level::trace, __VA_ARGS__)
