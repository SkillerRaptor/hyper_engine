/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use colored::Colorize;
use log::{Level, LevelFilter};
use std::path::Path;

const LOG_FOLDER: &str = "./logs/";

pub fn init() {
    if !Path::new(LOG_FOLDER).exists() {
        std::fs::create_dir(LOG_FOLDER).expect("Failed to create logs folder");
    }

    let log_level = if cfg!(debug_assertions) {
        LevelFilter::Debug
    } else {
        LevelFilter::Info
    };

    fern::Dispatch::new()
        .level(log_level)
        .chain(
            fern::Dispatch::new()
                .format(|out, message, record| {
                    let time = chrono::Local::now().format("%H:%M:%S");
                    let level = match record.level() {
                        Level::Error => "error".red(),
                        Level::Warn => "warn".bright_yellow(),
                        Level::Info => "info".green(),
                        Level::Debug => "debug".cyan(),
                        Level::Trace => "trace".purple(),
                    };

                    out.finish(format_args!("{} | {}: {}", time, level, message))
                })
                .chain(std::io::stdout()),
        )
        .chain(
            fern::Dispatch::new()
                .format(|out, message, record| {
                    let time = chrono::Local::now().format("%H:%M:%S");
                    let level = match record.level() {
                        Level::Error => "error",
                        Level::Warn => "warn",
                        Level::Info => "info",
                        Level::Debug => "debug",
                        Level::Trace => "trace",
                    };

                    out.finish(format_args!("{} | {}: {}", time, level, message))
                })
                .chain(
                    fern::log_file(format!(
                        "./logs/hyper_engine_{}.log",
                        chrono::Local::now().format("%d-%m-%Y_%H-%M-%S")
                    ))
                    .expect("Failed to create log file"),
                ),
        )
        .apply()
        .expect("Failed to create logger");

    let backtrace_mode = if cfg!(debug_assertions) {
        log_panics::BacktraceMode::Resolved
    } else {
        log_panics::BacktraceMode::Off
    };

    log_panics::Config::new()
        .backtrace_mode(backtrace_mode)
        .install_panic_hook();
}
