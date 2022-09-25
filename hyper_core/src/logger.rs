/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

pub use crate::errors::LoggerInitError;

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
    // Creates log folder if the folder does not exists
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

    let latest_log_file = format!("{}/latest.log", LOG_FOLDER,);
    let current_log_file = format!(
        "{}/hyper_engine_{}.log",
        LOG_FOLDER,
        Local::now().format("%d-%m-%Y_%H-%M-%S")
    );

    Dispatch::new()
        .level(log_level)
        .chain(
            // Creates dispatch for console logging
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
                // Chains the standard output
                .chain(io::stdout()),
        )
        .chain(
            // Creates dispatch for file logging
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
                    // Chains the latest.log file
                    std::fs::OpenOptions::new()
                        .write(true)
                        .create(true)
                        .truncate(true)
                        .open(&latest_log_file)
                        .map_err(|error| {
                            LoggerInitError::FileCreationFailure(latest_log_file, error)
                        })?,
                )
                .chain(
                    // Chains the log file for the current time
                    fern::log_file(&current_log_file).map_err(|error| {
                        LoggerInitError::FileCreationFailure(current_log_file, error)
                    })?,
                ),
        )
        .apply()?;
    Ok(())
}
