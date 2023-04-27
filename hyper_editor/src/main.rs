/*
 * Copyright (c) 2022-2023, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

mod editor;
mod logger;

use editor::Editor;

use hyper_engine::application::ApplicationBuilder;

use color_eyre::Result;
use logger::Verbosity;

fn main() -> Result<()> {
    color_eyre::install()?;

    // TODO: Implement CLI for '-debug' or '-trace' options
    logger::init(Verbosity::Debug)?;

    ApplicationBuilder::new()
        .title("HyperEditor")
        .width(1280)
        .height(720)
        .resizable(false)
        .build(Box::new(Editor::new()))?
        .run();

    Ok(())
}
