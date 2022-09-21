/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

pub use crate::error::LoggerInitError;

use chrono::Local;
use colored::Colorize;
use fern::Dispatch;
use log::{info, Level, LevelFilter};
use std::{fs, io, path::Path};
use tracing::instrument;

const LOG_FOLDER: &str = "./logs";

#[instrument(skip_all)]
pub fn init() -> Result<(), LoggerInitError> {
    create_logs_folder()?;
    create_dispatches()?;

    info!("Initialized logger");

    Ok(())
}

#[instrument(skip_all)]
fn create_logs_folder() -> Result<(), LoggerInitError> {
    if !Path::new(LOG_FOLDER).exists() {
        fs::create_dir(LOG_FOLDER).map_err(LoggerInitError::FolderCreationFailure)?;
    }

    Ok(())
}

#[instrument(skip_all)]
fn create_dispatches() -> Result<(), LoggerInitError> {
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
                        "{}/hyper_engine_{}.log",
                        LOG_FOLDER,
                        Local::now().format("%d-%m-%Y_%H-%M-%S")
                    ))
                    .map_err(|error| {
                        let file = format!(
                            "{}/hyper_engine_{}.log",
                            LOG_FOLDER,
                            Local::now().format("%d-%m-%Y_%H-%M-%S")
                        );
                        LoggerInitError::FileCreationFailure(file, error)
                    })?,
                ),
        )
        .apply()?;
    Ok(())
}
