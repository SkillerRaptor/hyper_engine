/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use chrono::Local;
use color_eyre::Result;
use fern::{
    colors::{Color, ColoredLevelConfig},
    Dispatch,
};
use log::LevelFilter;
use std::io;

#[allow(dead_code)]
#[derive(Clone, Copy, Debug)]
pub(crate) enum Verbosity {
    Error,
    Warning,
    Info,
    Debug,
    Trace,
}

pub(crate) fn init(verbosity: Verbosity) -> Result<()> {
    let levels = ColoredLevelConfig::new()
        .error(Color::Red)
        .warn(Color::BrightYellow)
        .info(Color::Green)
        .debug(Color::Cyan)
        .trace(Color::Magenta);

    let mut logger = Dispatch::new();

    logger = logger.format(move |out, message, record| {
        let reset = "\x1B[0m";

        let time = {
            let current_time = Local::now().format("%Y-%m-%d %H:%M:%S");
            let black = format!("\x1B[{}m", Color::BrightBlack.to_fg_str());
            format!("{black}[{current_time}]{reset}")
        };

        let level = {
            let current_level = record.level();
            let color = format!("\x1B[{}m", levels.get_color(&record.level()).to_fg_str());
            format!("{color}{current_level:<5}{reset}")
        };

        let message = format!("{reset}{message}");
        out.finish(format_args!("{time} {level} {message}"))
    });

    logger = match verbosity {
        Verbosity::Error => logger.level(LevelFilter::Error),
        Verbosity::Warning => logger.level(LevelFilter::Warn),
        Verbosity::Info => logger.level(LevelFilter::Info),
        Verbosity::Debug => logger.level(LevelFilter::Debug),
        Verbosity::Trace => logger.level(LevelFilter::Trace),
    };

    logger = logger.chain(io::stdout());

    logger.apply()?;

    Ok(())
}
