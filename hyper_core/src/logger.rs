/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use chrono::Local;
use colored::Colorize;
use fern::Dispatch;
use log::{Level, LevelFilter};
use std::{fs, io, path::Path};
use tracing::instrument;

const LOG_FOLDER: &str = "./logs/";

#[instrument(skip_all)]
pub fn init() {
    if !Path::new(LOG_FOLDER).exists() {
        fs::create_dir(LOG_FOLDER).unwrap_or_else(|_| panic!("Failed to create {}", LOG_FOLDER));
    }

    let log_level = if cfg!(debug_assertions) {
        LevelFilter::Debug
    } else {
        LevelFilter::Info
    };

    Dispatch::new()
        .level(log_level)
        .chain(
            Dispatch::new()
                .format(|out, message, record| {
                    let time = Local::now().format("%H:%M:%S");
                    let level = match record.level() {
                        Level::Error => "error".red(),
                        Level::Warn => "warn".bright_yellow(),
                        Level::Info => "info".green(),
                        Level::Debug => "debug".cyan(),
                        Level::Trace => "trace".purple(),
                    };

                    out.finish(format_args!("{} | {}: {}", time, level, message))
                })
                .chain(io::stdout()),
        )
        .chain(
            Dispatch::new()
                .format(|out, message, record| {
                    let time = Local::now().format("%H:%M:%S");
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
                        Local::now().format("%d-%m-%Y_%H-%M-%S")
                    ))
                    .expect("Failed to create log file"),
                ),
        )
        .apply()
        .expect("Failed to apply logger dispatch");
}
