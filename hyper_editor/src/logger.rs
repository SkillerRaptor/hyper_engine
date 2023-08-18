/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

use chrono::Local;
use fern::{
    colors::{Color, ColoredLevelConfig},
    Dispatch,
};
use log::{Level, LevelFilter};
use std::io;

#[derive(Clone, Copy, Debug)]
pub(crate) enum Verbosity {
    Error,
    Warning,
    Info,
    Debug,
    Trace,
}

pub(crate) fn init(verbosity: Verbosity) {
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
            let bright_black = format!("\x1B[{}m", Color::BrightBlack.to_fg_str());
            format!("{}[{}]{}", bright_black, current_time, reset)
        };

        let target = {
            let current_target = record.target().split_once(':');
            let white = format!("\x1B[{}m", Color::White.to_fg_str());
            format!("{}{:<15}{}", white, current_target.unwrap().0, reset)
        };

        let level = {
            let current_level = record.level();
            let color = format!("\x1B[{}m", levels.get_color(&record.level()).to_fg_str());
            format!("{}{:<5}{}", color, current_level, reset)
        };

        let color = if record.level() == Level::Trace {
            format!("\x1B[{}m", Color::White.to_fg_str())
        } else {
            format!("\x1B[{}m", Color::BrightWhite.to_fg_str())
        };
        let message = format!("{}{}", color, message);

        out.finish(format_args!(
            "{} {} {} {} {}",
            time, target, level, message, reset
        ))
    });

    let level_filter = match verbosity {
        Verbosity::Error => LevelFilter::Error,
        Verbosity::Warning => LevelFilter::Warn,
        Verbosity::Info => LevelFilter::Info,
        Verbosity::Debug => LevelFilter::Debug,
        Verbosity::Trace => LevelFilter::Trace,
    };

    logger = logger.level(level_filter);

    logger = logger.level_for("egui", LevelFilter::Off);

    logger = logger.level_for("egui_winit", LevelFilter::Off);

    logger = logger.chain(io::stdout());

    logger.apply().unwrap();
}
