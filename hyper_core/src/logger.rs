/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use chrono::Local;
use colored::Colorize;
use fern::Dispatch;
use log::{Level, LevelFilter};

const LOG_FOLDER: &str = "./logs/";

pub enum LoggerError {
    DispatchApplyError,
    FolderCreationError,
}

impl std::fmt::Display for LoggerError {
    fn fmt(&self, formatter: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            LoggerError::DispatchApplyError => {
                write!(formatter, "Applying dispatch has failed")
            }
            LoggerError::FolderCreationError => {
                write!(formatter, "Creating logs folder has failed")
            }
        }
    }
}

pub fn init() -> Result<(), LoggerError> {
    if !std::path::Path::new(LOG_FOLDER).exists() {
        std::fs::create_dir(LOG_FOLDER).map_err(|_| LoggerError::FolderCreationError)?;
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
                .chain(std::io::stdout()),
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
        .map_err(|_| LoggerError::DispatchApplyError)?;

    Ok(())
}
