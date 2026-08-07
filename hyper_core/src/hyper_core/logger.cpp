/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_core/logger.hpp"

#include <array>
#include <chrono>
#include <cstdio>
#include <fstream>
#include <string_view>

#include <fmt/chrono.h>

#include "hyper_core/assertion.hpp"

namespace he::logger {

struct LevelData {
    std::string_view label;
    std::string_view color;
};

static constexpr std::string_view s_reset = "\033[0m";
static constexpr std::string_view s_gray = "\033[38;2;120;120;120m";
static constexpr std::string_view s_white = "\033[38;2;211;211;211m";
static constexpr std::string_view s_dark_gray = "\033[38;2;69;69;69m";

static constexpr std::array<LevelData, 6> s_level_data = { {
    { .label = "trace", .color = "\033[38;2;128;0;128m" },
    { .label = "debug", .color = "\033[38;2;0;0;255m" },
    { .label = "info", .color = "\033[38;2;0;128;0m" },
    { .label = "warning", .color = "\033[38;2;255;215;0m" },
    { .label = "error", .color = "\033[38;2;255;0;0m" },
    { .label = "fatal", .color = "\033[38;2;220;20;60m" },
} };

static Level s_level = Level::Info;
static std::ofstream s_file;

void initialize(const Level level)
{
    s_level = level;
    s_file.open("latest.log", std::ios::out | std::ios::trunc);

    HE_INFO("Initialized logger");
}

void flush()
{
    std::fflush(stdout);

    if (s_file.is_open()) {
        s_file.flush();
    }
}

static bool is_enabled(const Level level) { return static_cast<u8>(level) <= static_cast<u8>(s_level); }

static std::string current_timestamp()
{
    const std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    return fmt::format("{:%Y-%m-%dT%H:%M:%S}", now);
}

void log(const Level level, const std::string_view message)
{
    if (!is_enabled(level)) {
        return;
    }

    const LevelData &data = s_level_data[static_cast<u8>(level)];
    const std::string timestamp = current_timestamp();

    fmt::print(
        "{}{} {}{}{}{}: {}{}{}\n",
        s_dark_gray,
        timestamp,
        data.color,
        data.label,
        s_reset,
        s_gray,
        s_white,
        message,
        s_reset);

    if (s_file.is_open()) {
        s_file << timestamp << ' ' << data.label << ": " << message << '\n';
    }
}

} // namespace he::logger
