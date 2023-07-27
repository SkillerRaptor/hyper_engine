/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#![cfg_attr(
    all(target_os = "windows", not(feature = "console")),
    windows_subsystem = "windows"
)]

mod editor;
mod logger;

use editor::Editor;

use hyper_engine::application::ApplicationBuilder;

use color_eyre::Result;
use logger::Verbosity;

fn main() -> Result<()> {
    color_eyre::install()?;

    if cfg!(feature = "console") {
        if cfg!(debug_assertions) {
            logger::init(Verbosity::Debug)?;
        } else {
            logger::init(Verbosity::Info)?;
        }
    }

    ApplicationBuilder::new()
        .title("HyperEditor")
        .width(1280)
        .height(720)
        .resizable(true)
        .build(Box::new(Editor::new()))?
        .run();

    Ok(())
}
