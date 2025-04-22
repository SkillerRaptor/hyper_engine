/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <memory>

#include <spdlog/spdlog.h>

class Logger
{
public:
    static void initialize();

    static std::shared_ptr<spdlog::logger> internal_logger();

private:
    static std::shared_ptr<spdlog::logger> s_internal_logger;
};

#define HE_INFO(...) SPDLOG_LOGGER_CALL(::Logger::internal_logger(), spdlog::level::info, __VA_ARGS__)
#define HE_WARN(...) SPDLOG_LOGGER_CALL(::Logger::internal_logger(), spdlog::level::warn, __VA_ARGS__)
#define HE_ERROR(...) SPDLOG_LOGGER_CALL(::Logger::internal_logger(), spdlog::level::err, __VA_ARGS__)
#define HE_CRITICAL(...) SPDLOG_LOGGER_CALL(::Logger::internal_logger(), spdlog::level::critical, __VA_ARGS__)
#define HE_DEBUG(...) SPDLOG_LOGGER_CALL(::Logger::internal_logger(), spdlog::level::debug, __VA_ARGS__)
#define HE_TRACE(...) SPDLOG_LOGGER_CALL(::Logger::internal_logger(), spdlog::level::trace, __VA_ARGS__)