/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

use hyper_engine::application::Application;

use chrono::Local;
use color_eyre::Result;
use colored::Colorize;
use fern::Dispatch;
use log::{Level, LevelFilter};
use std::{fs, io};

fn main() -> Result<()> {
    color_eyre::install()?;

    install_logger()?;

    let mut application = Application::new()?;
    application.run()?;

    Ok(())
}

fn install_logger() -> Result<()> {
    const LOGS_FOLDER: &str = "./logs";

    fs::create_dir_all(LOGS_FOLDER)?;

    let log_level = if cfg!(debug_assertions) {
        LevelFilter::Debug
    } else {
        LevelFilter::Info
    };

    let latest_log_file = format!("{}/latest.log", LOGS_FOLDER);
    let current_log_file = format!(
        "{}/hyper_engine_{}.log",
        LOGS_FOLDER,
        Local::now().format("%d-%m-%Y_%H-%M-%S")
    );

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
                    std::fs::OpenOptions::new()
                        .write(true)
                        .create(true)
                        .truncate(true)
                        .open(&latest_log_file)?,
                )
                .chain(fern::log_file(&current_log_file)?),
        )
        .apply()?;

    Ok(())
}
