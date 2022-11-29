/*
 * Copyright (c) 2022, SkillerRaptor
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

pub fn init(verbosity: usize) -> Result<()> {
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

        // TODO: Add command line option to disable thread id for sync code
        let thread_id = {
            let current_thread_id = std::thread::current().id();
            let white = format!("\x1B[{}m", Color::BrightMagenta.to_fg_str());
            let current_thread_id = format!("{current_thread_id:?}");
            format!("{white}{current_thread_id:<12}{reset}")
        };

        let level = {
            let current_level = record.level();
            let color = format!("\x1B[{}m", levels.get_color(&record.level()).to_fg_str());
            format!("{color}{current_level:<5}{reset}")
        };

        let message = format!("{reset}{message}");

        out.finish(format_args!("{time} {thread_id} {level} {message}"))
    });

    // TODO: Disable any logging from other crates

    logger = match verbosity {
        0 => logger.level(LevelFilter::Warn),
        1 => logger.level(LevelFilter::Info),
        2 => logger.level(LevelFilter::Debug),
        3 => logger.level(LevelFilter::Trace),
        _ => logger.level(LevelFilter::Error),
    };

    logger = logger.chain(io::stdout());

    logger.apply()?;

    Ok(())
}
